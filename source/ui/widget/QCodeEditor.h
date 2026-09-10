#pragma once
#ifndef QCODEEDITOR_H
#define QCODEEDITOR_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QMap>
#include <QStringList>

class QCodeEditorHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    enum FormatType
    {
        Variable,
        Function,
        Keyword1,
        Keyword2,
        Operator,
        Number,
        String,
        Comment
    };

    explicit QCodeEditorHighlighter(QTextDocument* parent = nullptr) : QSyntaxHighlighter(parent)
    {
        initDefaultFormats();
        addKeyword1("if");
        addKeyword1("else");
        addKeyword1("loop");
        addKeyword1("fn");
        addKeyword1("return");
        addKeyword2("null");
        addKeyword2("true");
        addKeyword2("false");
        addKeyword2("_macro_name");
        addKeyword2("_count");
        addKeyword2("_index");
        addKeyword2("_rand_last");
        addKeyword2("_cur_last_x");
        addKeyword2("_cur_last_y");
        addKeyword2("_cur_last_ax");
        addKeyword2("_cur_last_ay");
        addOperator("=");
        addOperator("+");
        addOperator("++");
        addOperator("+=");
        addOperator("-");
        addOperator("--");
        addOperator("-=");
        addOperator("*");
        addOperator("*=");
        addOperator("/");
        addOperator("/=");
        addOperator("%");
        addOperator("&");
        addOperator("|");
        addOperator("^");
        addOperator("<<");
        addOperator(">>");
        addOperator(">");
        addOperator("<");
        addOperator(">=");
        addOperator("<=");
        addOperator("==");
        addOperator("!=");
        addOperator("&&");
        addOperator("||");
        addOperator(":");
        updateRules();
    }

    void setColor(FormatType type, const QColor& color)
    {
        formats[type].setForeground(color);
    }

    void addKeyword1(const QString& keyword)
    {
        if (!keyword1List.contains(keyword)) keyword1List.append(keyword);
    }
    void addKeyword2(const QString& keyword)
    {
        if (!keyword2List.contains(keyword)) keyword2List.append(keyword);
    }
    void addOperator(const QString& op)
    {
        if (!operatorList.contains(op)) operatorList.append(op);
    }

    void updateRules()
    {
        highlightingRules.clear();
        HighlightingRule rule;

        rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z_][A-Za-z0-9_]*\\b"));
        rule.format = formats[Variable];
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()"));
        rule.format = formats[Function];
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression(QStringLiteral("\\b\\d+(\\.\\d+)?\\b"));
        rule.format = formats[Number];
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression(QStringLiteral("'[^']*'"));
        rule.format = formats[String];
        highlightingRules.append(rule);

        for (const QString& op : std::as_const(operatorList))
        {
            rule.pattern = QRegularExpression(QRegularExpression::escape(op));
            rule.format = formats[Operator];
            highlightingRules.append(rule);
        }

        for (const QString& kw : std::as_const(keyword2List))
        {
            rule.pattern = QRegularExpression(QStringLiteral("\\b") + QRegularExpression::escape(kw) + QStringLiteral("\\b"));
            rule.format = formats[Keyword2];
            highlightingRules.append(rule);
        }

        for (const QString& kw : std::as_const(keyword1List))
        {
            rule.pattern = QRegularExpression(QStringLiteral("\\b") + QRegularExpression::escape(kw) + QStringLiteral("\\b"));
            rule.format = formats[Keyword1];
            highlightingRules.append(rule);
        }

        rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
        rule.format = formats[Comment];
        highlightingRules.append(rule);
    }

protected:
    void highlightBlock(const QString& text) override
    {
        for (const HighlightingRule& rule : std::as_const(highlightingRules))
        {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext())
            {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> highlightingRules;
    QMap<FormatType, QTextCharFormat> formats;

    QStringList keyword1List;
    QStringList keyword2List;
    QStringList operatorList;

    void initDefaultFormats()
    {
        setColor(Variable, QColor("#80C0FF"));       // 浅蓝
        setColor(Function, QColor("#FFD080"));       // 浅黄
        setColor(Keyword1, QColor("#C080C0"));       // 紫
        setColor(Keyword2, QColor("#5080FF"));       // 蓝
        setColor(Operator, QColor("#FFE080"));       // 黄
        setColor(Number, QColor("#B0D0A0"));         // 浅绿
        setColor(String, QColor("#D09080"));         // 橘红
        setColor(Comment, QColor("#70A050"));        // 绿
    }
};

class QCodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    QCodeEditorHighlighter* highlighter;
public:
    QCodeEditor(QWidget* parent = nullptr) : QPlainTextEdit(parent), highlighter(new QCodeEditorHighlighter(document())) { setStyleSheet("background-color:#222"); }
    void setColor(QCodeEditorHighlighter::FormatType type, const QColor& color) { highlighter->setColor(type, color); }
    void addKeyword1(const QString& keyword) { highlighter->addKeyword1(keyword); }
    void addKeyword2(const QString& keyword) { highlighter->addKeyword2(keyword); }
    void addOperator(const QString& op) { highlighter->addOperator(op); }
    void updateRules() { highlighter->updateRules(); }
};

#endif