#ifndef HAPPYSAD_H
#define HAPPYSAD_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QImageCapture>
#include <QPushButton>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslSocket>
#include <QSslError>
#include <QUrl>
#include <QString>
#include <QPixmap>
#include <QTimer>

class HappySad : public QWidget
{
    Q_OBJECT

public:
    explicit HappySad(QWidget *parent = nullptr);
    ~HappySad();
    // Test Hume API connection
    void testHumeConnection();
    
    // Initialize UI and camera
    void initializeUI();
    void startCamera();
    void stopCamera();

private slots:
    // Capture photo from camera
    void capturePhoto();
    
    // Handle Hume API responses
    void onJobSubmitted(QNetworkReply *reply);
    void onJobStatusChecked(QNetworkReply *reply);
    void onJobResultsRetrieved(QNetworkReply *reply);
    
    // Display emotion results
    void displayEmotionResult(const QString &emotion, const QString &probabilities);
    
    // Network error handling
    void onNetworkError(QNetworkReply::NetworkError error);
    void onSslErrors(const QList<QSslError> &errors);
    
    // Camera capture slots
    void onImageSaved(int id, const QString &fileName);
    void onImageCaptureError(int id, QImageCapture::Error error, const QString &errorString);
    
    // Retry mechanism for status checking
    void checkJobStatus();

private:
    // Utility functions
    void submitPhotoToHumeAPI(const QString &imagePath);
    void setupConnections();
    QString parseHumeResponse(const QByteArray &jsonResponse);
    
    // Membres de la caméra
    QCamera *camera;
    QMediaCaptureSession *captureSession;
    QImageCapture *imageCapture;
    QVideoWidget *videoWidget;
    
    // Membres de l'interface
    QPushButton *btnCapture;
    QPushButton *btnStartStop;
    QLabel *lblResult;
    QLabel *lblEmotion;
    QLabel *lblImage;
    
    // Network and API
    QNetworkAccessManager *networkManager;
    QString humeApiKey;
    QString lastCapturedImagePath;
    QString currentJobId;
    QTimer *statusCheckTimer;
    int statusCheckRetries;
    
    // Constants
    static const QString HUME_API_URL;
    static const QString API_KEY;
    // Maximum number of times to poll job status before giving up
    static const int MAX_STATUS_RETRIES;
};

#endif // HAPPYSAD_H
