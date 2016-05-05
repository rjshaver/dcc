#include <QDebug>
#include <QtCore>
#include "FunctionViewWidget.h"
#include "ui_FunctionViewWidget.h"
#include "RenderTags.h"
//#include "XMLTYPE.h"
FunctionViewWidget::FunctionViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionViewWidget)
{
    ui->setupUi(this);
    m_current_rendering = new QTextDocument(ui->textEdit);
    m_doc_cursor = new QTextCursor(m_current_rendering);
    ui->textEdit->setTextBackgroundColor(Qt::black);
    m_current_format =  m_doc_cursor->blockFormat();
    m_current_format.setNonBreakableLines(true); // each block is single line
    m_current_format.setBackground(Qt::black);
    m_chars_format.setBackground(Qt::black);
    m_chars_format.setForeground(Qt::white);
    //ui->label->setTextFormat(Qt::RichText);
}

FunctionViewWidget::~FunctionViewWidget()
{
    delete ui;
}

void FunctionViewWidget::prtt(const char *s)
{
    m_doc_cursor->insertText(s);
    collected_text+=s;
    //collected_text+="<br>";
}
void FunctionViewWidget::prtt(const QString &s)
{
    m_doc_cursor->insertText(s);
    collected_text+=s;
    //collected_text+="<br>";
}

void FunctionViewWidget::addEOL()
{
    m_doc_cursor->insertBlock(m_current_format);
    m_doc_cursor->setBlockFormat(m_current_format);
}
void FunctionViewWidget::TAGbegin(TAG_TYPE tag_type, void *p)
{
    QColor col= RenderTag_2_Color(tag_type);

    switch(tag_type)
    {
        case XT_Function:
            m_current_rendering->clear();
            m_chars_format.setForeground(Qt::white);
            m_doc_cursor->setBlockFormat(m_current_format);
            m_doc_cursor->setCharFormat(m_chars_format);
            break;
        case XT_FuncName:
        case XT_Symbol:
        case XT_Keyword:
        case XT_DataType:
        case XT_Number:
        case XT_AsmOffset:
        case XT_AsmLabel:
            m_chars_format.setForeground(col);
            m_doc_cursor->setCharFormat(m_chars_format);
            break;
        default:
            qDebug()<<"Tag type:"<<tag_type;
    }
}
void FunctionViewWidget::TAGend(TAG_TYPE tag_type)
{
    switch(tag_type)
    {
        case XT_Function:
        {
            collected_text+="</body>";
            // TODO: What about attributes with spaces?
            QFile res("result.html");
            res.open(QFile::WriteOnly);
            res.write(m_current_rendering->toHtml().toUtf8());
            res.close();
            ui->textEdit->setDocument(m_current_rendering);
            collected_text.clear();
            break;
        }
        case XT_FuncName:
        case XT_Symbol:
        case XT_Keyword:
        case XT_DataType:
        case XT_Number:
        case XT_AsmOffset:
        case XT_AsmLabel:
            m_chars_format.setForeground(Qt::white);
            m_doc_cursor->setCharFormat(m_chars_format);
            m_doc_cursor->setBlockFormat(m_current_format);
            break;
        default:
            qDebug()<<"Tag end:"<<tag_type;
    }
}