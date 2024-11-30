#include <QtWidgets>

#include "mdichild.h"

MdiChild::MdiChild(QWidget *parent)
    : QWidget(parent) {
    setupEditor();
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(editor);
    setLayout(mainLayout);
}

void MdiChild::newFile() {
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &MdiChild::documentWasModified);

    editor->clear();
}

bool MdiChild::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QAlias"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    editor->setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &MdiChild::documentWasModified);

    return true;
}

bool MdiChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("QAlias"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::saveOnUpdate() {
    save();
}

void MdiChild::displayMessage(QString message) {
    if (message.isEmpty()) {
        messageBox->hide();
        return;
    }

    QLabel *messageLabel = messageBox->findChild<QLabel*>();
    if (messageLabel) {
        messageLabel->setText(message);
    }

    QTextCursor cursor = editor->textCursor();
    QRect rect = editor->cursorRect(cursor);
    QPoint pos = editor->mapToGlobal(rect.bottomLeft());

    messageBox->move(pos.x(), pos.y());
    messageBox->adjustSize();
    messageBox->show();
}

void MdiChild::documentWasModified()
{
    setWindowModified(editor->document()->isModified());
}

bool MdiChild::maybeSave()
{
    if (!editor->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("QAlias"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    editor->document()->setModified(false);
    highlighter->filename = fileName;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MdiChild::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(18);

    editor = new CodeEditor;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());
    connect(editor, &CodeEditor::contentsChanged, highlighter, &Highlighter::updateData);
    connect(editor, &CodeEditor::contentsChanged, this, &MdiChild::saveOnUpdate);

    messageBox = new QWidget(this);
    messageBox->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    messageBox->setStyleSheet("background-color: lightyellow; border: 1px solid gray;");
    QLabel *messageLabel = new QLabel("", messageBox);
    QVBoxLayout *boxLayout = new QVBoxLayout(messageBox);
    boxLayout->addWidget(messageLabel);
    messageBox->setLayout(boxLayout);
    messageBox->hide();
    connect(highlighter, &Highlighter::displayError, this, &MdiChild::displayMessage);
}
