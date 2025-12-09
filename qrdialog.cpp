// QR Dialog implementation removed — functionality replaced by voice-to-text feature
// The original qrdialog.cpp was intentionally removed from the build. Kept as a placeholder
// to preserve historical edits in the workspace. See voicetotext.* and voicetotextdialog.* for
// the new voice chat / speech-to-text integration.

// No runtime code in this file.
QRDialog::QRDialog(QWidget *parent)
    : QDialog(parent)
    , tableWidget(nullptr)
    , btnLoadImage(nullptr)
    , btnScanImage(nullptr)
    , btnGenSelected(nullptr)
    , btnGenAll(nullptr)
    , btnSaveQr(nullptr)
    , lblQrImage(nullptr)
    , comboGenerator(nullptr)
    , lblPreview(nullptr)
    , lblDecoded(nullptr)
    , networkManager(nullptr)
{
    // Main vertical layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Top area: children table
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7);
    QStringList headers = {"ID","Nom", "Age", "Allergie", "Extra", "Parent", "Email"};
    tableWidget->setHorizontalHeaderLabels(headers);
    mainLayout->addWidget(tableWidget, 3);

    // Bottom area: preview + controls
    QHBoxLayout *bottom = new QHBoxLayout();
    // preview
    lblPreview = new QLabel(this);
    lblPreview->setMinimumSize(240,160);
    lblPreview->setAlignment(Qt::AlignCenter);
    lblPreview->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    lblPreview->setText(tr("Aucune image chargée"));
    bottom->addWidget(lblPreview, 1);

    QVBoxLayout *controls = new QVBoxLayout();
    btnLoadImage = new QPushButton(tr("Charger une image..."), this);
    btnScanImage = new QPushButton(tr("Scanner QR depuis image"), this);
    // generation buttons
    btnGenSelected = new QPushButton(tr("Générer QR (sélection)"), this);
    btnGenAll = new QPushButton(tr("Générer QR (tous les enfants)"), this);
    btnSaveQr = new QPushButton(tr("Enregistrer QR"), this);
    btnSaveQr->setEnabled(false);
    // preview for generated QR
    lblQrImage = new QLabel(this);
    lblQrImage->setMinimumSize(240,240);
    lblQrImage->setAlignment(Qt::AlignCenter);
    lblQrImage->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    lblQrImage->setText(tr("QR généré affichera ici"));
    btnScanImage->setEnabled(false);
    lblDecoded = new QLabel(tr("Contenu QR: (vide)"), this);
    lblDecoded->setWordWrap(true);

    controls->addWidget(btnLoadImage);
    controls->addWidget(btnScanImage);
    // generator mode selection
    comboGenerator = new QComboBox(this);
    comboGenerator->addItem(tr("Local (offline)"));
    comboGenerator->addItem(tr("QR Server (api.qrserver.com)"));
    comboGenerator->addItem(tr("QuickChart (quickchart.io)"));
    controls->addWidget(comboGenerator);
    controls->addSpacing(6);
    controls->addWidget(btnGenSelected);
    controls->addWidget(btnGenAll);
    controls->addWidget(btnSaveQr);
    controls->addWidget(lblDecoded);
    controls->addStretch();
    bottom->addLayout(controls, 2);
    bottom->addWidget(lblQrImage, 1);

    mainLayout->addLayout(bottom, 1);

    networkManager = new QNetworkAccessManager(this);

    // populate list from DB
    populateChildrenList();

    connect(btnLoadImage, &QPushButton::clicked, this, &QRDialog::onLoadImage);
    connect(btnScanImage, &QPushButton::clicked, this, &QRDialog::onScanImage);
    connect(btnGenSelected, &QPushButton::clicked, this, [this](){
        // Generate QR for selected child, falling back to first row if none
        int row = tableWidget->currentRow();
        if (row < 0 && tableWidget->rowCount() > 0) row = 0;
        if (row < 0) { QMessageBox::information(this, tr("Générer QR"), tr("Aucun enfant disponible à générer")); return; }
        QString id = tableWidget->item(row, 0)->text();
        QString nom = tableWidget->item(row, 1)->text();
        QString age = tableWidget->item(row, 2)->text();
        QString parent = tableWidget->item(row, 5)->text();
        QString email = tableWidget->item(row, 6)->text();
        // Use a human-readable payload so phone scanners display friendly info
        QString payload = QString("ID: %1\nNom: %2\nAge: %3\nParent: %4\nEmail: %5").arg(id, nom, age, parent, email);

        QString mode = comboGenerator ? comboGenerator->currentText() : QString("Local (offline)");
        if (mode.contains("QR Server", Qt::CaseInsensitive)) {
            // Use QR Server API to generate this single QR — replace newlines for a safe URL payload
            QString apiPayload = payload;
            apiPayload.replace('\n', ' ');
            QString url = QString("https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(apiPayload)));
            QNetworkRequest req{QUrl(url)};
            // Verify SSL availability before attempting HTTPS requests
            if (!QSslSocket::supportsSsl()) {
                QMessageBox::warning(this, tr("Erreur SSL"), tr("Le support SSL n'est pas disponible. Impossible d'utiliser les API HTTPS. Activez OpenSSL ou choisissez le mode 'Local (offline)'."));
                return;
            }
            req.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/QRDialog-API")));
            QNetworkReply *reply = networkManager->get(req);
            // capture variables for reply handler
            {
                QString out = QString("%1/qr_%2.png").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator() + QString("QR CODES"), id);
                reply->setProperty("outPath", QVariant(out));
            }
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                QByteArray data = reply->readAll();
                QNetworkReply::NetworkError netErr = reply->error();
                int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                QVariant ctVar = reply->header(QNetworkRequest::ContentTypeHeader);
                QString contentType = ctVar.toString();

                // Check network error first
                if (netErr != QNetworkReply::NoError) {
                    QString err = reply->errorString();
                    reply->deleteLater();
                    QMessageBox::warning(this, tr("Erreur réseau"), tr("Erreur réseau lors de la génération: %1").arg(err));
                    return;
                }

                // Check HTTP status
                if (httpStatus < 200 || httpStatus >= 300) {
                    QString body = QString::fromUtf8(data);
                    reply->deleteLater();
                    QMessageBox::warning(this, tr("Erreur API"), tr("HTTP %1: %2").arg(QString::number(httpStatus), body.left(1024)));
                    return;
                }

                // Ensure reply is an image
                if (!contentType.startsWith("image")) {
                    QString body = QString::fromUtf8(data);
                    reply->deleteLater();
                    QMessageBox::warning(this, tr("Erreur API"), tr("Le serveur n'a pas renvoyé une image: %1\n%2").arg(contentType, body.left(1024)));
                    return;
                }

                QPixmap p;
                if (!p.loadFromData(data)) {
                    reply->deleteLater();
                    QMessageBox::warning(this, tr("Erreur"), tr("Données reçues invalides (image attendue)."));
                    return;
                }

                // Save & show
                generatedQrPixmap = p;
                lblQrImage->setPixmap(p.scaled(lblQrImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                btnSaveQr->setEnabled(true);
                QString out = reply->property("outPath").toString();
                if (!out.isEmpty()) {
                    QDir d(QFileInfo(out).path());
                    if (!d.exists()) d.mkpath(d.path());
                    if (p.save(out)) {
                        QMessageBox::information(this, tr("QR sauvegardé"), tr("QR généré et sauvegardé: %1").arg(out));
                    } else {
                        QMessageBox::warning(this, tr("QR sauvegardé"), tr("QR généré mais impossible à enregistrer: %1").arg(out));
                    }
                }

                reply->deleteLater();
            });
            return; // handled via network
        } else if (mode.contains("QuickChart", Qt::CaseInsensitive)) {
            QString apiPayload = payload;
            apiPayload.replace('\n', ' ');
            QString url = QString("https://quickchart.io/qr?text=%1&size=300").arg(QString::fromUtf8(QUrl::toPercentEncoding(apiPayload)));
            QNetworkRequest req{QUrl(url)};
            if (!QSslSocket::supportsSsl()) {
                QMessageBox::warning(this, tr("Erreur SSL"), tr("Le support SSL n'est pas disponible. Impossible d'utiliser les API HTTPS. Activez OpenSSL ou choisissez le mode 'Local (offline)'."));
                return;
            }
            req.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/QRDialog-QuickChart")));
            QNetworkReply *reply = networkManager->get(req);
            {
                QString out = QString("%1/qr_%2.png").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator() + QString("QR CODES"), id);
                reply->setProperty("outPath", QVariant(out));
            }
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                QByteArray data = reply->readAll();
                reply->deleteLater();
                QPixmap p;
                if (p.loadFromData(data)) {
                    generatedQrPixmap = p;
                    lblQrImage->setPixmap(p.scaled(lblQrImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    btnSaveQr->setEnabled(true);
                    QString out = reply->property("outPath").toString();
                    if (!out.isEmpty()) {
                        QDir d(QFileInfo(out).path());
                        if (!d.exists()) d.mkpath(d.path());
                        p.save(out);
                        QMessageBox::information(this, tr("QR sauvegardé"), tr("QR généré et sauvegardé: %1").arg(out));
                    }
                } else {
                    QMessageBox::warning(this, tr("Erreur"), tr("Impossible de générer le QR via QuickChart."));
                }
            });
            return;
        }

        // Generate locally using qrcodegen
        try {
            ::qrcodegen::QrCode q = ::qrcodegen::QrCode::encodeText(payload.toStdString(), ::qrcodegen::QrCode::Ecc::LOW);
            int s = q.getSize();
            int scale = 6; // pixels per module
            int margin = 4; // quiet zone in modules — important for phone scanners
            int imgSize = (s + 2 * margin) * scale;
            QImage img(imgSize, imgSize, QImage::Format_ARGB32);
            img.fill(Qt::white);
            for (int y = 0; y < s; ++y) {
                for (int x = 0; x < s; ++x) {
                    QColor col = q.getModule(x, y) ? Qt::black : Qt::white;
                    for (int yy = 0; yy < scale; ++yy) for (int xx = 0; xx < scale; ++xx) img.setPixelColor((margin + x)*scale + xx, (margin + y)*scale + yy, col);
                }
            }
            QPixmap p = QPixmap::fromImage(img);
            generatedQrPixmap = p;
            lblQrImage->setPixmap(p.scaled(lblQrImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            btnSaveQr->setEnabled(true);

            // Also save automatically into Desktop/QR CODES
            QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString dir = desktop + QDir::separator() + "QR CODES";
            if (!QDir(dir).exists()) QDir().mkpath(dir);
            QString safeName = QString("qr_%1.png").arg(id);
            QString outPath = dir + QDir::separator() + safeName;
            if (generatedQrPixmap.save(outPath)) {
                QMessageBox::information(this, tr("QR sauvegardé"), tr("QR généré et sauvegardé: %1").arg(outPath));
            }
        } catch (...) {
            QMessageBox::warning(this, tr("Erreur"), tr("Impossible de générer le QR localement."));
        }
    });

    connect(btnGenAll, &QPushButton::clicked, this, [this]() {
        // Build combined payload for all visible rows
        QStringList lines;
        for (int r = 0; r < tableWidget->rowCount(); ++r) {
            if (tableWidget->isRowHidden(r)) continue;
            QString id = tableWidget->item(r, 0)->text();
            QString name = tableWidget->item(r, 1)->text();
            lines << QString("%1:%2").arg(id, name);
        }
        if (lines.isEmpty()) { QMessageBox::information(this, tr("Générer QR"), tr("Aucun enfant visible à inclure.")); return; }

        QString payload = lines.join("|");
        QUrl u(QString("https://api.qrserver.com/v1/create-qr-code/?size=400x400&data=%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(payload))));
        QNetworkRequest rq(u);
        rq.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/QRDialog-Generator")));

        // Generate combined payload locally
        QString mode = comboGenerator ? comboGenerator->currentText() : QString("Local (offline)");
        try {
            // Generate one QR per visible child and save to Desktop/QR CODES
            QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString dir = desktop + QDir::separator() + "QR CODES";
            if (!QDir(dir).exists()) QDir().mkpath(dir);

            for (int r = 0; r < tableWidget->rowCount(); ++r) {
                if (tableWidget->isRowHidden(r)) continue;
                QString id = tableWidget->item(r, 0)->text();
                QString nom = tableWidget->item(r, 1)->text();
                QString age = tableWidget->item(r, 2)->text();
                QString parent = tableWidget->item(r, 5)->text();
                QString email = tableWidget->item(r, 6)->text();
                QString perPayload = QString("ID: %1\nNom: %2\nAge: %3\nParent: %4\nEmail: %5").arg(id, nom, age, parent, email);
                if (mode.contains("QR Server", Qt::CaseInsensitive) || mode.contains("QuickChart", Qt::CaseInsensitive)) {
                    // Use API to request the generated image
                    QString apiUrl;
                    if (mode.contains("QR Server", Qt::CaseInsensitive)) {
                        QString safe = perPayload; safe.replace('\n', ' ');
                        apiUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(safe)));
                    } else {
                        QString safe = perPayload; safe.replace('\n', ' ');
                        apiUrl = QString("https://quickchart.io/qr?text=%1&size=300").arg(QString::fromUtf8(QUrl::toPercentEncoding(safe)));
                    }
                    QNetworkRequest req{QUrl(apiUrl)};
                    if (!QSslSocket::supportsSsl()) {
                        qDebug() << "Skipping batch API request: SSL not available" << apiUrl;
                        continue;
                    }
                    req.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/QRDialog-BatchApi")));
                    // make a request and capture id/name
                    QString outPath = dir + QDir::separator() + QString("%1_%2.png").arg(id, nom.replace(QRegularExpression("[^A-Za-z0-9_\\-]"), "_"));
                    QNetworkReply *reply = networkManager->get(req);
                    reply->setProperty("outPath", outPath);
                    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                        QByteArray data = reply->readAll();
                        QNetworkReply::NetworkError netErr = reply->error();
                        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        QVariant ctVar = reply->header(QNetworkRequest::ContentTypeHeader);
                        QString contentType = ctVar.toString();

                        if (netErr != QNetworkReply::NoError) {
                            qDebug() << "Batch API network error:" << reply->errorString() << "for" << reply->url();
                            reply->deleteLater();
                            return;
                        }
                        if (httpStatus < 200 || httpStatus >= 300) {
                            QString body = QString::fromUtf8(data);
                            qDebug() << "Batch API HTTP error" << httpStatus << "for" << reply->url() << body.left(512);
                            reply->deleteLater();
                            return;
                        }
                        if (!contentType.startsWith("image")) {
                            QString body = QString::fromUtf8(data);
                            qDebug() << "Batch API returned non-image content for" << reply->url() << contentType << body.left(512);
                            reply->deleteLater();
                            return;
                        }

                        QPixmap p;
                        if (p.loadFromData(data)) {
                            if (!reply->property("outPath").toString().isEmpty()) {
                                QString out = reply->property("outPath").toString();
                                QDir d(QFileInfo(out).path()); if (!d.exists()) d.mkpath(d.path());
                                if (!p.save(out)) {
                                    qDebug() << "Failed to save batch QR" << out;
                                }
                            }
                        } else {
                            qDebug() << "Failed to load QR data for batch item" << reply->url();
                        }

                        reply->deleteLater();
                    });
                    continue;
                }

                // fallback local generation
                ::qrcodegen::QrCode q = ::qrcodegen::QrCode::encodeText(perPayload.toStdString(), ::qrcodegen::QrCode::Ecc::MEDIUM);
                int s = q.getSize();
                int scale = 6;
                int margin = 4;
                int imgSize = (s + 2 * margin) * scale;
                QImage img(imgSize, imgSize, QImage::Format_ARGB32);
                img.fill(Qt::white);
                for (int y = 0; y < s; ++y) {
                    for (int x = 0; x < s; ++x) {
                        QColor col = q.getModule(x, y) ? Qt::black : Qt::white;
                        for (int yy = 0; yy < scale; ++yy) for (int xx = 0; xx < scale; ++xx) img.setPixelColor((margin + x)*scale + xx, (margin + y)*scale + yy, col);
                    }
                }
                QPixmap p = QPixmap::fromImage(img);
                // save each as ID_Name.png safe filename
                QString safe = nom;
                // sanitize file name: replace characters not allowed in file names with '_'
                safe.replace(QRegularExpression("[^A-Za-z0-9_\\-]"), "_");
                QString outPath = dir + QDir::separator() + QString("%1_%2.png").arg(id, safe);
                if (!p.save(outPath)) {
                    qDebug() << "Failed to save" << outPath;
                }
            }

            // Show feedback: show last generated QR in preview
            ::qrcodegen::QrCode qLast = ::qrcodegen::QrCode::encodeText(payload.toStdString(), ::qrcodegen::QrCode::Ecc::MEDIUM);
            int s = qLast.getSize();
            int scale = 6;
            int margin = 4;
            int imgSize = (s + 2 * margin) * scale;
            QImage img(imgSize, imgSize, QImage::Format_ARGB32);
            img.fill(Qt::white);
            for (int y = 0; y < s; ++y) {
                for (int x = 0; x < s; ++x) {
                    QColor col = qLast.getModule(x, y) ? Qt::black : Qt::white;
                    for (int yy = 0; yy < scale; ++yy) for (int xx = 0; xx < scale; ++xx) img.setPixelColor((margin + x)*scale + xx, (margin + y)*scale + yy, col);
                }
            }
            QPixmap p = QPixmap::fromImage(img);
            generatedQrPixmap = p;
            lblQrImage->setPixmap(p.scaled(lblQrImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            btnSaveQr->setEnabled(true);
            QMessageBox::information(this, tr("QR générés"), tr("QRs générés et sauvegardés dans: %1").arg(dir));
        } catch (...) {
            QMessageBox::warning(this, tr("Erreur"), tr("Impossible de générer le QR pour tous."));
        }
    });

    connect(btnSaveQr, &QPushButton::clicked, this, &QRDialog::onSaveQr);
    connect(networkManager, &QNetworkAccessManager::finished, this, &QRDialog::onScanReplyFinished);
}

void QRDialog::populateChildrenList()
{
    tableWidget->setRowCount(0);
    QSqlQuery q;
    q.exec("SELECT id_enfant, nom_prenom, age, allergie_remarque, '' AS extra, nom_prenom_parents, email FROM genfant");
    int row = 0;
    while (q.next()) {
        tableWidget->insertRow(row);
        for (int c = 0; c < 7; ++c) {
            QString text = q.value(c).toString();
            QTableWidgetItem *it = new QTableWidgetItem(text);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(row, c, it);
        }
        ++row;
    }
    tableWidget->resizeColumnsToContents();
}

void QRDialog::onLoadImage()
{
    QString img = QFileDialog::getOpenFileName(this, tr("Choisir une image contenant un QR"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (img.isEmpty()) return;
    currentImagePath = img;
    QPixmap px(img);
    if (!px.isNull()) {
        lblPreview->setPixmap(px.scaled(lblPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        btnScanImage->setEnabled(true);
        lblDecoded->setText(tr("Contenu QR: prêt à scanner..."));
    } else {
        lblPreview->setText(tr("Impossible d'ouvrir l'image"));
        btnScanImage->setEnabled(false);
    }
}

void QRDialog::onScanImage()
{
    if (currentImagePath.isEmpty()) return;

    // Use the public QR decoding API https://api.qrserver.com/v1/read-qr-code/
    QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QFile *file = new QFile(currentImagePath);
    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier image pour l'envoi."));
        delete file;
        delete multi;
        return;
    }

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(QFileInfo(*file).fileName())));
    imagePart.setBodyDevice(file);
    file->setParent(multi);
    multi->append(imagePart);

    QNetworkRequest req{QUrl("https://api.qrserver.com/v1/read-qr-code/")};
    req.setHeader(QNetworkRequest::UserAgentHeader, QVariant(QStringLiteral("Qt/QRDialog")));

    lblDecoded->setText(tr("Envoi pour décodage en cours..."));
    btnScanImage->setEnabled(false);

    QNetworkReply *reply = networkManager->post(req, multi);
    // multi now owned by reply
    multi->setParent(reply);
}

void QRDialog::onScanReplyFinished(QNetworkReply *reply)
{
    // We always receive finished for any request from this manager; ensure it came from our POST
    if (!reply) return;

    QByteArray body = reply->readAll();
    reply->deleteLater();

    // parse result: API returns an array
    QJsonDocument doc = QJsonDocument::fromJson(body);
    if (!doc.isArray()) {
        lblDecoded->setText(tr("Réponse invalide du serveur de décodage."));
        btnScanImage->setEnabled(!currentImagePath.isEmpty());
        return;
    }

    QJsonArray arr = doc.array();
    if (arr.isEmpty()) {
        lblDecoded->setText(tr("Aucun résultat retourné."));
        btnScanImage->setEnabled(!currentImagePath.isEmpty());
        return;
    }

    // Usually the first element has 'symbol' array and first symbol's 'data' has decoded text
    QString decoded;
    QJsonObject first = arr[0].toObject();
    if (first.contains("symbol")) {
        QJsonArray sym = first["symbol"].toArray();
        if (!sym.isEmpty()) {
            QJsonObject s0 = sym[0].toObject();
            decoded = s0.value("data").toString();
            if (decoded.isEmpty()) {
                // sometimes 'data' may be null and 'error' has message
                decoded = s0.value("error").toString();
            }
        }
    }

    if (decoded.isEmpty()) decoded = tr("Décodage échoué ou QR non reconnu.");

    showDecoded(decoded);
    btnScanImage->setEnabled(!currentImagePath.isEmpty());
}

void QRDialog::showDecoded(const QString &decoded)
{
    lblDecoded->setText(tr("Contenu QR: %1").arg(decoded));

    // If decoded text looks like an integer id, try to find a matching child and select it
    QString d = decoded.trimmed();
    bool ok = false;
    int id = d.toInt(&ok);
    if (ok) {
        for (int r = 0; r < tableWidget->rowCount(); ++r) {
            QTableWidgetItem *it = tableWidget->item(r, 0);
            if (!it) continue;
            if (it->text() == QString::number(id)) {
                tableWidget->selectRow(r);
                tableWidget->scrollToItem(it);

                QString nom = tableWidget->item(r, 1) ? tableWidget->item(r,1)->text() : QString();
                QString age = tableWidget->item(r, 2) ? tableWidget->item(r,2)->text() : QString();
                QString allergie = tableWidget->item(r, 3) ? tableWidget->item(r,3)->text() : QString();
                QString parent = tableWidget->item(r, 5) ? tableWidget->item(r,5)->text() : QString();
                QString email = tableWidget->item(r, 6) ? tableWidget->item(r,6)->text() : QString();
                QString details = tr("ID: %1\nNom: %2\nAge: %3\nAllergie: %4\nParent: %5\nEmail: %6")
                        .arg(QString::number(id), nom, age, allergie, parent, email);
                QMessageBox::information(this, tr("Enfant trouvé"), details);
                return;
            }
        }
    }

    // Otherwise try to match by name
    for (int r = 0; r < tableWidget->rowCount(); ++r) {
        for (int c = 0; c < tableWidget->columnCount(); ++c) {
            QTableWidgetItem *it = tableWidget->item(r, c);
            if (!it) continue;
            if (it->text().contains(d, Qt::CaseInsensitive)) {
                tableWidget->selectRow(r);
                tableWidget->scrollToItem(it);

                QString idStr = tableWidget->item(r,0) ? tableWidget->item(r,0)->text() : QString();
                QString nom = tableWidget->item(r, 1) ? tableWidget->item(r,1)->text() : QString();
                QString age = tableWidget->item(r, 2) ? tableWidget->item(r,2)->text() : QString();
                QString allergie = tableWidget->item(r, 3) ? tableWidget->item(r,3)->text() : QString();
                QString parent = tableWidget->item(r, 5) ? tableWidget->item(r,5)->text() : QString();
                QString email = tableWidget->item(r, 6) ? tableWidget->item(r,6)->text() : QString();
                QString details = tr("ID: %1\nNom: %2\nAge: %3\nAllergie: %4\nParent: %5\nEmail: %6")
                        .arg(idStr, nom, age, allergie, parent, email);
                QMessageBox::information(this, tr("Enfant trouvé"), details);
                return;
            }
        }
    }

    // If we didn't find a match, but decoded text contains key=value pairs, try to parse and show a basic message
    QRegularExpression kvRe("([A-Za-z0-9_]+)=([^;|\\n]+)");
    QRegularExpressionMatchIterator itKv = kvRe.globalMatch(decoded);
    if (itKv.hasNext()) {
        QStringList parts;
        while (itKv.hasNext()) {
            auto m = itKv.next();
            parts << QString("%1: %2").arg(m.captured(1), m.captured(2));
        }
        QMessageBox::information(this, tr("QR décodé"), parts.join("\n"));
        return;
    }
}

void QRDialog::onGeneratedImageFinished()
{
    // kept for compatibility - image finishing handled per-reply in lambdas
}

void QRDialog::onSaveQr()
{
    if (generatedQrPixmap.isNull()) {
        QMessageBox::information(this, tr("Enregistrer QR"), tr("Aucun QR généré à enregistrer."));
        return;
    }

    // Default to Desktop/QR CODES
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString dir = desktop + QDir::separator() + "QR CODES";
    if (!QDir(dir).exists()) QDir().mkpath(dir);
    QString def = dir + QDir::separator() + "qr_code.png";
    QString fn = QFileDialog::getSaveFileName(this, tr("Enregistrer QR"), def, tr("Images PNG (*.png);;JPEG (*.jpg *.jpeg)"));
    if (fn.isEmpty()) return;

    // Choose format based on extension
    QString ext = QFileInfo(fn).suffix().toLower();
    if (ext == "jpg" || ext == "jpeg") {
        if (!generatedQrPixmap.save(fn, "JPG")) QMessageBox::warning(this, tr("Enregistrer"), tr("Échec enregistrement JPEG."));
    } else {
        if (!generatedQrPixmap.save(fn, "PNG")) QMessageBox::warning(this, tr("Enregistrer"), tr("Échec enregistrement PNG."));
    }
}
