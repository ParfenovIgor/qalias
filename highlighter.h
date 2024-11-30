#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpPart>
#include <QFileInfo>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

struct ColorInfo {
    int positionBegin, positionEnd;
    QString color;
};

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);
    void updateData();
    QString filename;

protected:
    void highlightBlock(const QString &text) override;

signals:
    void displayError(QString error);

private:
    QNetworkAccessManager *managerHighlight;
    QNetworkAccessManager *managerCheckerrors;

    QString lastText;
    QMap <int, QList <ColorInfo>> highlightData;
    QMap <int, QList <ColorInfo>> errorData;
    void replyFinishedHighlight(QNetworkReply *reply);
    void replyFinishedCheckerrors(QNetworkReply *reply);
};
