#ifndef VOICETOTEXTDIALOG_H
#define VOICETOTEXTDIALOG_H

#include <QDialog>

class QPushButton;
class QLabel;
class QTextEdit;
class VoiceToText;

class VoiceToTextDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VoiceToTextDialog(QWidget *parent = nullptr);
    ~VoiceToTextDialog() override;

private slots:
    void onStart();
    void onStop();

    void onStatusChanged(const QString &s);
    void onInterim(const QString &t);
    void onFinal(const QString &role, const QString &text);
    void onError(const QString &err);

private:
    VoiceToText *m_engine = nullptr;
    QPushButton *m_startBtn = nullptr;
    QPushButton *m_stopBtn = nullptr;
    QLabel *m_status = nullptr;
    QTextEdit *m_text = nullptr;
};

#endif // VOICETOTEXTDIALOG_H
