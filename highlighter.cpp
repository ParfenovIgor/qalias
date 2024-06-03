#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    manager = new QNetworkAccessManager();
    
    connect(manager,
            &QNetworkAccessManager::finished,
            this,
            &Highlighter::replyFinished);
}

void Highlighter::replyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "QNetworkReply error: " << reply->errorString();
        return;
    }
    highlightData.clear();

    QString answer = reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(answer.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        return;
    }
    QJsonArray jsonArray = jsonDoc.array();
    for (int i = 0; i < jsonArray.size(); i++) {
        QJsonObject jsonObj = jsonArray[i].toObject();
        int lineBegin = jsonObj["lb"].toInt();
        int positionBegin = jsonObj["pb"].toInt();
        int lineEnd = jsonObj["le"].toInt();
        int positionEnd = jsonObj["pe"].toInt();
        QString color = jsonObj["cl"].toString();
        if (lineBegin == lineEnd) {
            highlightData[lineBegin].append({positionBegin, positionEnd, color});
        }
        else {
            highlightData[lineBegin].append({positionBegin, document()->findBlockByLineNumber(lineBegin).text().size() - 1, color});
            for (int i = lineBegin + 1; i <= lineEnd - 1; i++) {
                highlightData[i].append({0, document()->findBlockByLineNumber(i).text().size() - 1, color});
            }
            highlightData[lineEnd].append({0, positionEnd, color});
        }
    }
    rehighlight();
}

void Highlighter::updateData() {
    if (document()->toPlainText() == lastText) return;
    lastText = document()->toPlainText();
    
    QUrl url("http://127.0.0.1:12913/highlight");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    manager->post(QNetworkRequest(request), document()->toPlainText().toUtf8());
}

void Highlighter::highlightBlock(const QString &text) {
    for (ColorInfo colorInfo : highlightData[currentBlock().blockNumber()]) {
        QTextCharFormat format;
        format.setForeground(QColor(colorInfo.color));
        setFormat(colorInfo.positionBegin, colorInfo.positionEnd - colorInfo.positionBegin + 1, format);
    }
}
