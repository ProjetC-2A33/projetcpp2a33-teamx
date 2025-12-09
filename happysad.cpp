#include "happysad.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaDevices>
#include <QVideoSink>
#include <QImageCapture>
#include <QStandardPaths>
#include <QtGlobal>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDir>
#include <QImage>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QDebug>
#include <QMessageBox>
#include <QSslSocket>
#include <QSslError>
#include <QNetworkProxyFactory>

// Initialization of static constants
const QString HappySad::HUME_API_URL = "https://api.hume.ai/v0/batch/jobs";
const QString HappySad::API_KEY = "nCkDcVxCcO78RGwrW2UvZxHJaA0PAxAh7t0tv0kW6TikIudO";
// After 60 tries (2s interval = ~120s) we'll stop polling and report a timeout to the user
const int HappySad::MAX_STATUS_RETRIES = 60;

HappySad::HappySad(QWidget *parent)
    : QWidget(parent),
      camera(nullptr),
      captureSession(nullptr),
      videoWidget(nullptr),
      btnCapture(nullptr),
      btnStartStop(nullptr),
      lblResult(nullptr),
      lblEmotion(nullptr),
      lblImage(nullptr),
      imageCapture(nullptr),
      networkManager(nullptr),
      humeApiKey(API_KEY),
      currentJobId(""),
      statusCheckTimer(nullptr),
      statusCheckRetries(0)
{
    initializeUI();
    setupConnections();
}

HappySad::~HappySad()
{
    if (camera) {
        stopCamera();
    }
    if (statusCheckTimer) {
        statusCheckTimer->stop();
        delete statusCheckTimer;
    }
    if (networkManager) {
        delete networkManager;
    }
}

void HappySad::initializeUI()
{
    // Créer le layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // === TITRE ===
    QLabel *titleLabel = new QLabel("Détection d'Émotions");
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2C4B74;");
    mainLayout->addWidget(titleLabel);

    // === LAYOUT HORIZONTAL POUR VIDÉO ET RÉSULTATS ===
    QHBoxLayout *contentLayout = new QHBoxLayout();

    // --- Section Caméra ---
    QVBoxLayout *cameraLayout = new QVBoxLayout();
    
    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumSize(400, 300);
    videoWidget->setStyleSheet("background-color: black;");
    cameraLayout->addWidget(videoWidget);

    // --- Buttons for camera ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    btnStartStop = new QPushButton("Démarrer Caméra", this);
    btnStartStop->setMinimumHeight(35);
    btnStartStop->setStyleSheet(
        "QPushButton {"
        "   background-color: #5FC9C5;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4BA8A4;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3D8B87;"
        "}"
    );
    buttonLayout->addWidget(btnStartStop);

    btnCapture = new QPushButton("📷 Capturer & Analyser", this);
    btnCapture->setMinimumHeight(35);
    btnCapture->setStyleSheet(
        "QPushButton {"
        "   background-color: #F39C12;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #D68910;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #B8860B;"
        "}"
    );
    btnCapture->setEnabled(false);
    buttonLayout->addWidget(btnCapture);

    // Add "Test Hume" button for diagnostics
    QPushButton *btnTestHume = new QPushButton("🧪 Tester Hume API", this);
    btnTestHume->setMinimumHeight(35);
    btnTestHume->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #21618c;"
        "}"
    );
    connect(btnTestHume, &QPushButton::clicked, this, &HappySad::testHumeConnection);
    buttonLayout->addWidget(btnTestHume);

    cameraLayout->addLayout(buttonLayout);
    contentLayout->addLayout(cameraLayout, 1);

    // --- Section Résultats ---
    QVBoxLayout *resultLayout = new QVBoxLayout();
    
    QLabel *resultTitleLabel = new QLabel("Résultats");
    QFont resultTitleFont;
    resultTitleFont.setPointSize(12);
    resultTitleFont.setBold(true);
    resultTitleLabel->setFont(resultTitleFont);
    resultTitleLabel->setStyleSheet("color: #2C4B74;");
    resultLayout->addWidget(resultTitleLabel);

    // Image capturée
    lblImage = new QLabel(this);
    lblImage->setMinimumSize(250, 200);
    lblImage->setMaximumSize(250, 200);
    lblImage->setAlignment(Qt::AlignCenter);
    lblImage->setStyleSheet("border: 2px solid #5FC9C5; background-color: white;");
    lblImage->setText("Aucune image capturée");
    resultLayout->addWidget(lblImage);

    // Émotion détectée
    lblEmotion = new QLabel(this);
    lblEmotion->setAlignment(Qt::AlignCenter);
    QFont emotionFont;
    emotionFont.setPointSize(14);
    emotionFont.setBold(true);
    lblEmotion->setFont(emotionFont);
    // Use high-contrast black text for maximum readability
    lblEmotion->setStyleSheet("color: black; background-color: transparent; padding: 6px; border-radius: 4px;");
    lblEmotion->setText("En attente...");
    resultLayout->addWidget(lblEmotion);

    // Détails des résultats
    lblResult = new QLabel(this);
    lblResult->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblResult->setWordWrap(true);
    // Make the text in the result box dark and easy to read, with a subtle border
    lblResult->setStyleSheet("color: black; background-color: #ffffff; border: 1px solid #e0e0e0; padding: 10px; border-radius: 5px;");
    lblResult->setText("Les résultats de l'analyse s'afficheront ici");
    resultLayout->addWidget(lblResult);

    resultLayout->addStretch();
    contentLayout->addLayout(resultLayout, 1);

    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch();

    // Initialiser le gestionnaire réseau
    networkManager = new QNetworkAccessManager(this);
}

void HappySad::setupConnections()
{
    // Connect button signals
    connect(btnStartStop, &QPushButton::clicked, this, &HappySad::startCamera);
    connect(btnCapture, &QPushButton::clicked, this, &HappySad::capturePhoto);
    
    // Check SSL support
    if (!QSslSocket::supportsSsl()) {
        lblResult->setText("Attention: SSL library not available. Install OpenSSL to enable HTTPS connections.");
        qDebug() << "OpenSSL unavailable:" << QSslSocket::sslLibraryVersionString();
    } else {
        qDebug() << "OpenSSL available:" << QSslSocket::sslLibraryVersionString();
    }

    // Use system proxy settings
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    
    // Initialize timer for status checks (Hume API uses async job-based processing)
    statusCheckTimer = new QTimer(this);
    connect(statusCheckTimer, &QTimer::timeout, this, &HappySad::checkJobStatus);
}

void HappySad::startCamera()
{
    if (!camera) {
        // Obtenir la caméra par défaut
        QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
        
        if (cameras.isEmpty()) {
            QMessageBox::critical(this, "Erreur", "Aucune caméra détectée sur cet appareil.");
            return;
        }

        camera = new QCamera(cameras[0], this);
        captureSession = new QMediaCaptureSession(this);
        captureSession->setCamera(camera);
        captureSession->setVideoOutput(videoWidget);

        // Setup image capture for high-quality stills
        // QImageCapture in Qt6 can be constructed without camera; attach it to the capture session
        imageCapture = new QImageCapture(this);
        captureSession->setImageCapture(imageCapture);
        connect(imageCapture, &QImageCapture::imageSaved, this, &HappySad::onImageSaved);
        // connect to the error signal in a way compatible across Qt versions
    #if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        connect(imageCapture, &QImageCapture::errorOccurred, this, &HappySad::onImageCaptureError);
    #else
        connect(imageCapture, &QImageCapture::error, this, &HappySad::onImageCaptureError);
    #endif

        btnCapture->setEnabled(true);
        btnStartStop->setText("Arrêter Caméra");
        
        camera->start();
        lblResult->setText("Caméra démarrée. Cliquez sur 'Capturer & Analyser' pour prendre une photo.");
    } else {
        // Arrêter la caméra
        stopCamera();
    }
}

void HappySad::stopCamera()
{
    if (camera) {
        camera->stop();
        delete camera;
        camera = nullptr;
        delete captureSession;
        captureSession = nullptr;
        if (imageCapture) {
            delete imageCapture;
            imageCapture = nullptr;
        }
        
        btnCapture->setEnabled(false);
        btnStartStop->setText("Démarrer Caméra");
        lblResult->setText("Caméra arrêtée.");
    }
}

void HappySad::capturePhoto()
{
    if (!camera) {
        QMessageBox::warning(this, "Avertissement", "La caméra n'est pas activée.");
        return;
    }

    // Obtenir le répertoire des images
    QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString imagePath = picturesPath + "/happysad_" + timestamp + ".jpg";

    // Créer une capture vidéo depuis le widget vidéo
    // Pour capturer depuis QVideoWidget, nous utilisons une approche alternative
    
    // Sauvegarder le chemin pour l'API
    lastCapturedImagePath = imagePath;

    if (imageCapture) {
        // Utiliser QImageCapture pour capturer en haute qualité et sauver le fichier
        int id = imageCapture->captureToFile(imagePath);
        Q_UNUSED(id);
        lblResult->setText("Capture en cours... sauvegarde en cours...");
    } else {
        // Fallback: screenshot du widget vidéo si QImageCapture non disponible
        QPixmap screenshot = videoWidget->grab();
        
        if (screenshot.save(imagePath, "JPG")) {
            lblResult->setText("Image capturée. Analyse en cours...");
            lblImage->setPixmap(screenshot.scaled(QSize(250,200), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            // Envoyer l'image à l'API Hume
            submitPhotoToHumeAPI(imagePath);
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de capturer l'image.");
        }
    }
}

void HappySad::submitPhotoToHumeAPI(const QString &imagePath)
{
    QFile *imageFile = nullptr;
    QTemporaryFile *tempFile = nullptr;

    // If image is large, compress it to reduce upload size
    const qint64 maxSize = 1 * 1024 * 1024; // 1 MB
    QFileInfo fi(imagePath);
    if (fi.exists() && fi.size() > maxSize) {
        QImage img(imagePath);
        if (!img.isNull()) {
            int maxDim = 1280;
            img = img.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            tempFile = new QTemporaryFile(QDir::tempPath() + "/happysad_XXXXXX.jpg");
            if (tempFile->open()) {
                img.save(tempFile, "JPG", 75);
                tempFile->flush();
                tempFile->seek(0);
                imageFile = tempFile;
            } else {
                delete tempFile;
                tempFile = nullptr;
                imageFile = new QFile(imagePath);
            }
        } else {
            imageFile = new QFile(imagePath);
        }
    } else {
        imageFile = new QFile(imagePath);
    }

    if (!imageFile->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Unable to open captured image.");
        lblResult->setText("Error: Cannot open image.");
        delete imageFile;
        return;
    }

    // Create multipart request for Hume API
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Add the photo file
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, 
                       QVariant("form-data; name=\"file\"; filename=\"photo.jpg\""));
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setBodyDevice(imageFile);
    imageFile->setParent(multiPart);
    multiPart->append(imagePart);

    // Add JSON configuration for models
    QHttpPart jsonPart;
    jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"json\""));
    jsonPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    
    QString jsonData = R"({
    "models": {
        "face": {}
    }
})";
    jsonPart.setBody(jsonData.toUtf8());
    multiPart->append(jsonPart);

    // Create request
    QNetworkRequest request{QUrl(HUME_API_URL)};
    request.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/HappySad")));
    // Hume API uses X-Hume-Api-Key header
    request.setRawHeader(QByteArray("X-Hume-Api-Key"), humeApiKey.toUtf8());
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));

    // Debug logging
    qDebug() << "Uploading file to Hume API:" << imageFile->fileName() << "size:" << QFileInfo(imageFile->fileName()).size();
    qDebug() << "Request headers:";
    for (const QByteArray &h : request.rawHeaderList()) {
        qDebug() << h << ":" << request.rawHeader(h);
    }
    if (tempFile) {
        tempFile->setParent(nullptr); // Will be cleaned up after reply finishes
    }

    // Submit job
    QNetworkReply *reply = networkManager->post(request, multiPart);
    multiPart->setParent(reply);

    // Connect signals
    connect(reply, &QNetworkReply::sslErrors, this, &HappySad::onSslErrors);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onJobSubmitted(reply);
    });
    
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError err){
        Q_UNUSED(err)
        qDebug() << "Immediate Network Error:" << reply->errorString();
        QByteArray body = reply->readAll();
        qDebug() << "Response body:" << body;
        QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (status.isValid()) qDebug() << "HTTP status:" << status.toInt();
        foreach(const QByteArray &h, reply->rawHeaderList()) {
            qDebug() << "Header:" << h << ":" << reply->rawHeader(h);
        }
        lblResult->setText("Network error: " + reply->errorString());
    });
    
    connect(reply, &QNetworkReply::errorOccurred, this, &HappySad::onNetworkError);

    lastCapturedImagePath = imagePath;
}

void HappySad::onJobSubmitted(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Error submitting job: %1").arg(reply->errorString());
        qDebug() << errorMsg;
        QByteArray body = reply->readAll();
        qDebug() << "Server response:" << body;
        lblResult->setText(errorMsg);
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    qDebug() << "Job submission response:" << responseData;

    // Parse job ID from response
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isObject()) {
        lblResult->setText("Error: Invalid response format");
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    currentJobId = jsonObj["job_id"].toString();
    
    if (currentJobId.isEmpty()) {
        lblResult->setText("Error: No job ID in response");
        reply->deleteLater();
        return;
    }

    qDebug() << "Job submitted successfully. Job ID:" << currentJobId;
    lblResult->setText(QString("Job submitted. ID: %1\nWaiting for analysis...").arg(currentJobId));

    // Start polling for job status
    statusCheckRetries = 0;
    statusCheckTimer->start(2000); // Check every 2 seconds

    reply->deleteLater();
}

void HappySad::checkJobStatus()
{
    if (currentJobId.isEmpty()) {
        statusCheckTimer->stop();
        return;
    }
    // Count this poll to allow a graceful timeout if the job stays queued too long
    statusCheckRetries++;
    if (statusCheckRetries > MAX_STATUS_RETRIES) {
        statusCheckTimer->stop();
        lblResult->setText(QString("Timeout waiting for job completion after %1 attempts.").arg(statusCheckRetries));
        qDebug() << "Hume job status check timeout for job:" << currentJobId;
        return;
    }
    QString statusUrl = QString("https://api.hume.ai/v0/batch/jobs/%1").arg(currentJobId);
    QNetworkRequest request{QUrl(statusUrl)};
    request.setRawHeader(QByteArray("X-Hume-Api-Key"), humeApiKey.toUtf8());
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));

    qDebug() << "Checking job status (attempt" << statusCheckRetries << "):" << statusUrl;

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onJobStatusChecked(reply);
    });
    connect(reply, &QNetworkReply::errorOccurred, this, &HappySad::onNetworkError);
}

void HappySad::onJobStatusChecked(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Error checking status: %1").arg(reply->errorString());
        qDebug() << errorMsg;
        statusCheckRetries++;
        if (statusCheckRetries > 30) { // Stop after 60 seconds of trying
            statusCheckTimer->stop();
            lblResult->setText("Error: Timeout waiting for job completion");
        }
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    qDebug() << "Status check response:" << responseData;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isObject()) {
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    
    // Status is nested in "state" object
    QString status;
    if (jsonObj.contains("state")) {
        QJsonObject stateObj = jsonObj["state"].toObject();
        status = stateObj["status"].toString();
    }

    qDebug() << "Job status (" << statusCheckRetries << "):" << status;
    // Show the status and progress attempt count to the user so it's clearer what's happening
    lblResult->setText(QString("Job status: %1 (attempt %2/%3)").arg(status, QString::number(statusCheckRetries), QString::number(MAX_STATUS_RETRIES)));

    if (status == "COMPLETED") {
        statusCheckTimer->stop();
        
        // Retrieve job results
        QString resultsUrl = QString("https://api.hume.ai/v0/batch/jobs/%1/predictions").arg(currentJobId);
        QNetworkRequest request{QUrl(resultsUrl)};
        request.setRawHeader(QByteArray("X-Hume-Api-Key"), humeApiKey.toUtf8());
        request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));

        QNetworkReply *resultsReply = networkManager->get(request);
        connect(resultsReply, &QNetworkReply::finished, this, [this, resultsReply]() {
            onJobResultsRetrieved(resultsReply);
        });
        connect(resultsReply, &QNetworkReply::errorOccurred, this, &HappySad::onNetworkError);

    } else if (status == "FAILED") {
        statusCheckTimer->stop();
        QString error = jsonObj["error"].toString();
        lblResult->setText(QString("Job failed: %1").arg(error));
    }

    reply->deleteLater();
}

void HappySad::onJobResultsRetrieved(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Error retrieving results: %1").arg(reply->errorString());
        qDebug() << errorMsg;
        lblResult->setText(errorMsg);
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    qDebug() << "Job results:" << responseData;

    QString emotionAnalysis = parseHumeResponse(responseData);
    
    if (!emotionAnalysis.isEmpty()) {
        displayEmotionResult("", emotionAnalysis);
    } else {
        lblResult->setText("Error: Unable to process API response");
    }

    reply->deleteLater();
}

void HappySad::onImageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id)
    lblResult->setText("Image saved. Submitting to Hume API for analysis...");

    // Display the image
    QPixmap pixmap(fileName);
    if (!pixmap.isNull()) {
        lblImage->setPixmap(pixmap.scaled(QSize(250,200), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Submit the image to Hume API
    submitPhotoToHumeAPI(fileName);
}

void HappySad::onImageCaptureError(int id, QImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id)
    Q_UNUSED(error)
    QString msg = QString("Erreur capture: %1").arg(errorString);
    lblResult->setText(msg);
    qDebug() << msg;
}

QString HappySad::parseHumeResponse(const QByteArray &jsonResponse)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonResponse);
    
    if (!jsonDoc.isArray()) {
        qDebug() << "Invalid Hume response format - expected array";
        return "";
    }

    QJsonArray sourceArray = jsonDoc.array();
    if (sourceArray.isEmpty()) {
        return "No predictions in response";
    }

    QString result;
    
    // Process each source (typically one per image submission)
    for (int s = 0; s < sourceArray.size(); ++s) {
        QJsonObject sourceObj = sourceArray[s].toObject();
        
        if (!sourceObj.contains("results")) {
            result += "No results in source\n";
            continue;
        }

        QJsonObject resultsObj = sourceObj["results"].toObject();
        if (!resultsObj.contains("predictions")) {
            result += "No predictions in results\n";
            continue;
        }

        QJsonArray predictionsArray = resultsObj["predictions"].toArray();
        if (predictionsArray.isEmpty()) {
            result += "No predictions array\n";
            continue;
        }

        // Process each prediction
        for (int p = 0; p < predictionsArray.size(); ++p) {
            QJsonObject prediction = predictionsArray[p].toObject();
            
            // Navigate to face models
            if (!prediction.contains("models")) {
                result += "No models in prediction\n";
                continue;
            }

            QJsonObject modelsObj = prediction["models"].toObject();
            if (!modelsObj.contains("face")) {
                result += "No face data in prediction\n";
                continue;
            }

            QJsonObject faceObj = modelsObj["face"].toObject();
            if (!faceObj.contains("grouped_predictions")) {
                result += "No grouped predictions in face data\n";
                continue;
            }

            QJsonArray groupedPredictionsArray = faceObj["grouped_predictions"].toArray();
            if (groupedPredictionsArray.isEmpty()) {
                result += "No faces detected in image\n";
                continue;
            }

            result += QString("Faces detected: %1\n\n").arg(groupedPredictionsArray.size());

            // Process each face group
            for (int g = 0; g < groupedPredictionsArray.size(); ++g) {
                QJsonObject groupObj = groupedPredictionsArray[g].toObject();
                
                if (!groupObj.contains("predictions")) {
                    continue;
                }

                QJsonArray facePredictionsArray = groupObj["predictions"].toArray();
                
                // Process each face prediction
                for (int f = 0; f < facePredictionsArray.size(); ++f) {
                    QJsonObject facePred = facePredictionsArray[f].toObject();
                    
                    result += QString("Face %1:\n").arg(f + 1);

                    // Get emotions array
                    if (facePred.contains("emotions")) {
                        QJsonArray emotionsArray = facePred["emotions"].toArray();
                        
                        QString maxEmotion;
                        double maxScore = 0.0;

                        result += "Emotions:\n";
                        
                        for (const QJsonValue &emotionVal : emotionsArray) {
                            QJsonObject emotionObj = emotionVal.toObject();
                            QString name = emotionObj["name"].toString();
                            double score = emotionObj["score"].toDouble();

                            result += QString("  • %1: %2%\n")
                                .arg(name, QString::number(score * 100, 'f', 1));

                            if (score > maxScore) {
                                maxScore = score;
                                maxEmotion = name;
                            }
                        }

                        // Add dominant emotion with emoji
                        QString emoji;
                        if (maxEmotion.contains("Joy", Qt::CaseInsensitive)) emoji = "😊";
                        else if (maxEmotion.contains("Sadness", Qt::CaseInsensitive)) emoji = "😢";
                        else if (maxEmotion.contains("Anger", Qt::CaseInsensitive)) emoji = "😠";
                        else if (maxEmotion.contains("Surprise", Qt::CaseInsensitive)) emoji = "😲";
                        else if (maxEmotion.contains("Fear", Qt::CaseInsensitive)) emoji = "😨";
                        else if (maxEmotion.contains("Disgust", Qt::CaseInsensitive)) emoji = "🤢";
                        else if (maxEmotion.contains("Awe", Qt::CaseInsensitive)) emoji = "🤩";
                        else if (maxEmotion.contains("Confusion", Qt::CaseInsensitive)) emoji = "😕";
                        else if (maxEmotion.contains("Boredom", Qt::CaseInsensitive)) emoji = "😐";
                        else if (maxEmotion.contains("Concentration", Qt::CaseInsensitive)) emoji = "🤔";
                        else emoji = "😐";

                        result += QString("\n🎯 Dominant: %1 %2 (%3%)\n\n")
                            .arg(maxEmotion, emoji, QString::number(maxScore * 100, 'f', 1));
                    }

                    // Get face box coordinates
                    if (facePred.contains("box")) {
                        QJsonObject boxObj = facePred["box"].toObject();
                        double x = boxObj["x"].toDouble();
                        double y = boxObj["y"].toDouble();
                        double w = boxObj["w"].toDouble();
                        double h = boxObj["h"].toDouble();
                        
                        result += QString("  Box: X=%1, Y=%2, W=%3, H=%4\n\n")
                            .arg(x, 0, 'f', 1).arg(y, 0, 'f', 1).arg(w, 0, 'f', 1).arg(h, 0, 'f', 1);
                    }
                }
            }
        }
    }

    return result;
}

void HappySad::displayEmotionResult(const QString &emotion, const QString &probabilities)
{
    if (!emotion.isEmpty()) {
        // Show the dominant emotion in the header with clear style
        lblEmotion->setText(QString("%1").arg(emotion));
    }
    // Format the probabilities block to use a clean readable block — keep line breaks
    QString formatted = probabilities;
    // Ensure we always use black text for results in case this is called externally
    lblResult->setStyleSheet("color: black; background-color: #ffffff; border: 1px solid #e0e0e0; padding: 10px; border-radius: 5px;");
    lblResult->setText(formatted);
}

void HappySad::onNetworkError(QNetworkReply::NetworkError error)
{
    qDebug() << "Erreur réseau:" << error;
    QString errorMsg = QString("Erreur de connexion réseau: Code %1").arg(error);
    lblResult->setText(errorMsg);
}

void HappySad::onSslErrors(const QList<QSslError> &errors)
{
    QStringList list;
    for (const QSslError &e : errors) {
        list << e.errorString();
        qDebug() << "SSL error:" << e.error() << e.errorString();
    }
    QString msg = "Erreur SSL: " + list.join("; ");
    lblResult->setText(msg + " — Vérifiez si OpenSSL est installé et accessible par l'exécutable.");
}

void HappySad::testHumeConnection()
{
    // Test connection to Hume API with a minimal request
    qDebug() << "Testing Hume API connection...";
    lblResult->setText("Testing Hume API connection...");

    QString testUrl = "https://api.hume.ai/v0/batch/jobs";
    QNetworkRequest request{QUrl(testUrl)};
    request.setRawHeader(QByteArray("X-Hume-Api-Key"), humeApiKey.toUtf8());
    request.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/HappySad-Test")));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    // Send minimal JSON POST to test connectivity
    QByteArray testData = R"({"models":{"face":{}}})";
    QNetworkReply *reply = networkManager->post(request, testData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        qDebug() << "Hume test finished; error:" << reply->errorString();
        QByteArray body = reply->readAll();
        qDebug() << "Response:" << body;
        
        if (reply->error() == QNetworkReply::NoError) {
            lblResult->setText("✓ Hume API connection successful!");
        } else {
            lblResult->setText("Test Hume: " + reply->errorString());
        }
        reply->deleteLater();
    });
    connect(reply, &QNetworkReply::errorOccurred, this, [this, reply](QNetworkReply::NetworkError){
        qDebug() << "Hume test immediate error: " << reply->errorString();
        lblResult->setText("Hume API error: " + reply->errorString());
    });
}
