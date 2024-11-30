#pragma once

#include "highlighter.h"
#include "codeeditor.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class MdiChild : public QWidget {
    Q_OBJECT

public:
    MdiChild(QWidget *parent = 0);
    CodeEditor *editor;
    Highlighter *highlighter;

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void saveOnUpdate();
    void displayMessage(QString message);

private slots:
    void documentWasModified();

private:
    void setupEditor();

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QWidget *messageBox;
    QString curFile;
    bool isUntitled;
};
