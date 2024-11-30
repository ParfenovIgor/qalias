#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    managerHighlight = new QNetworkAccessManager();
    managerCheckerrors = new QNetworkAccessManager();
    
    connect(managerHighlight,
            &QNetworkAccessManager::finished,
            this,
            &Highlighter::replyFinishedHighlight);

    connect(managerCheckerrors,
            &QNetworkAccessManager::finished,
            this,
            &Highlighter::replyFinishedCheckerrors);
}

void Highlighter::replyFinishedHighlight(QNetworkReply *reply) {
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

void Highlighter::replyFinishedCheckerrors(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "QNetworkReply error: " << reply->errorString();
        return;
    }
    errorData.clear();

    QString answer = reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(answer.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qWarning() << "Parse error at " << parseError.offset << ":" << parseError.errorString();
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    if (jsonObj["res"].toString() == "fail") {
        int lineBegin = jsonObj["lb"].toInt();
        int positionBegin = jsonObj["pb"].toInt();
        int lineEnd = jsonObj["le"].toInt();
        int positionEnd = jsonObj["pe"].toInt();
        QString color = "#FF8080";
        QString errorFilename = jsonObj["file"].toString();
        QString errorMessage = jsonObj["err"].toString();

        if (errorFilename == filename) {
            if (lineBegin == lineEnd) {
                errorData[lineBegin].append({positionBegin, positionEnd, color});
            }
            else {
                errorData[lineBegin].append({positionBegin, document()->findBlockByLineNumber(lineBegin).text().size() - 1, color});
                for (int i = lineBegin + 1; i <= lineEnd - 1; i++) {
                    errorData[i].append({0, document()->findBlockByLineNumber(i).text().size() - 1, color});
                }
                errorData[lineEnd].append({0, positionEnd, color});
            }
            displayError(errorMessage);
        }
        else {
            displayError("File: " + errorFilename + "\n" + errorMessage);
        }
    }
    else {
        displayError("");
    }
    rehighlight();
}

void Highlighter::updateData() {
    if (document()->toPlainText() == lastText) return;
    lastText = document()->toPlainText();
    
    {
        QUrl url("http://127.0.0.1:12913/highlight");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        managerHighlight->post(QNetworkRequest(request), document()->toPlainText().toUtf8());
    }
    {
        QUrl url("http://127.0.0.1:12913/checkerrors");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QString buffer = "calias -v " + filename;
        managerCheckerrors->post(QNetworkRequest(request), buffer.toUtf8());
    }
}

void Highlighter::highlightBlock(const QString &text) {
    for (ColorInfo colorInfo : highlightData[currentBlock().blockNumber()]) {
        QTextCharFormat format;
        format.setForeground(QColor(colorInfo.color));
        setFormat(colorInfo.positionBegin, colorInfo.positionEnd - colorInfo.positionBegin + 1, format);
    }
    for (ColorInfo colorInfo : errorData[currentBlock().blockNumber()]) {
        QTextCharFormat format;
        format.setUnderlineColor(QColor(colorInfo.color));
        format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        setFormat(colorInfo.positionBegin, colorInfo.positionEnd - colorInfo.positionBegin + 1, format);
    }
}
