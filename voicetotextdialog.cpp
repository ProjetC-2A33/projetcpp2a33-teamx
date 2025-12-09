#include "voicetotextdialog.h"
#include "voicetotext.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>

VoiceToTextDialog::VoiceToTextDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Voice -> Text (Hume)"));
    setModal(true);

    QVBoxLayout *main = new QVBoxLayout(this);

    m_status = new QLabel(tr("Not connected"), this);
    m_text = new QTextEdit(this);
    m_text->setReadOnly(true);

    QHBoxLayout *hl = new QHBoxLayout();
    m_startBtn = new QPushButton(tr("Start"), this);
    m_stopBtn = new QPushButton(tr("Stop"), this);
    m_stopBtn->setEnabled(false);

    hl->addWidget(m_startBtn);
    hl->addWidget(m_stopBtn);

    main->addWidget(m_status);
    main->addWidget(m_text, 1);
    main->addLayout(hl);

    m_engine = new VoiceToText(this);

    // Wire signals
    connect(m_startBtn, &QPushButton::clicked, this, &VoiceToTextDialog::onStart);
    connect(m_stopBtn, &QPushButton::clicked, this, &VoiceToTextDialog::onStop);

    connect(m_engine, &VoiceToText::statusChanged, this, &VoiceToTextDialog::onStatusChanged);
    connect(m_engine, &VoiceToText::interimMessageReceived, this, &VoiceToTextDialog::onInterim);
    connect(m_engine, &VoiceToText::finalMessageReceived, this, &VoiceToTextDialog::onFinal);
    connect(m_engine, &VoiceToText::errorOccurred, this, &VoiceToTextDialog::onError);

    resize(700, 420);
}

VoiceToTextDialog::~VoiceToTextDialog() = default;

void VoiceToTextDialog::onStart()
{
    // Use environment variable HUME_API_KEY if present
    QByteArray keyBytes;
    QString envKey = qgetenv("HUME_API_KEY");
    if (!envKey.isEmpty()) m_engine->setApiKey(envKey);

    if (!m_engine->hasApiKey()) {
        QMessageBox::warning(this, tr("Missing key"), tr("HUME_API_KEY not set in environment. Please set it or configure the app with a key."));
        return;
    }

    if (m_engine->start()) {
        m_status->setText(tr("Connecting / recording…"));
        m_startBtn->setEnabled(false);
        m_stopBtn->setEnabled(true);
        m_text->append(tr("--- Session started ---"));
    }
}

void VoiceToTextDialog::onStop()
{
    m_engine->stop();
    m_status->setText(tr("Stopped"));
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_text->append(tr("--- Session stopped ---\n"));
}

void VoiceToTextDialog::onStatusChanged(const QString &s)
{
    m_status->setText(s);
}

void VoiceToTextDialog::onInterim(const QString &t)
{
    // show interim messages in italics
    QString it = QString("<i>%1</i>").arg(t.toHtmlEscaped());
    m_text->append(it);
}

void VoiceToTextDialog::onFinal(const QString &role, const QString &text)
{
    QString p = QString("<b>%1:</b> %2").arg(role.toHtmlEscaped(), text.toHtmlEscaped());
    m_text->append(p);
}

void VoiceToTextDialog::onError(const QString &err)
{
    QMessageBox::warning(this, tr("Voice->Text error"), err);
    m_status->setText(tr("Error: %1").arg(err));
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
}
