#ifndef QPLAINTEXTBROWSERLOGGER_H
#define QPLAINTEXTBROWSERLOGGER_H

#include <QApplication>
#include <QTextBrowser>
#include <QDebug>
#include <QMutex>

class QPlainTextEditLogger {
    QTextBrowser * textBrowser;
    QMutex mutex;
public:
    explicit QPlainTextEditLogger(QTextBrowser *browser) : textBrowser(browser) {
        // Устанавливаем QTextBrowser как серийный вывод
        qInstallMessageHandler(QPlainTextEditLogger::messageHandler);
    }

    ~QPlainTextEditLogger() {
        // Отключаем обработчик сообщений
        qInstallMessageHandler(nullptr);
       // qDebug() << "exit is InstallMessageHandler";
    }

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {

        // Создаем объект QPlainTextEditLogger, который будет использоваться для вывода сообщения
        QPlainTextEditLogger* logger = static_cast<QPlainTextEditLogger*>(qApp->property("logger").value<void*>());

        if (logger) {
            QMutexLocker locker(&logger->mutex); // Блокируем доступ к текстовому браузеру

            QString formattedMessage;
            switch (type) {
            case QtDebugMsg:
                formattedMessage = "[DEBUG] " + msg;
                break;
            case QtWarningMsg:
                formattedMessage = "[WARNING] " + msg;
                break;
            case QtCriticalMsg:
                formattedMessage = "[CRITICAL] " + msg;
                break;
            case QtFatalMsg:
                formattedMessage = "[FATAL] " + msg;
                break;
            case QtInfoMsg:
                formattedMessage = "[INFO] " + msg;
                break;
            }
            // Добавляем сообщение в QTextBrowser
            logger->textBrowser->append(formattedMessage);
        }
    }
};

#endif // QPLAINTEXTBROWSERLOGGER_H
