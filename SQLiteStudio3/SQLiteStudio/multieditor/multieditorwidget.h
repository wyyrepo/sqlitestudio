#ifndef MULTIEDITORWIDGET_H
#define MULTIEDITORWIDGET_H

#include <QWidget>

class MultiEditorWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit MultiEditorWidget(QWidget *parent = 0);

        virtual void setValue(const QVariant& value) = 0;
        virtual QVariant getValue() = 0;
        virtual void setReadOnly(bool value) = 0;
        virtual QList<QWidget*> getNoScrollWidgets() = 0;

        void installEventFilter(QObject* filterObj);

        bool isUpToDate() const;
        void setUpToDate(bool value);

    private:
        bool upToDate = true;

    signals:
        void valueModified();
};

#endif // MULTIEDITORWIDGET_H