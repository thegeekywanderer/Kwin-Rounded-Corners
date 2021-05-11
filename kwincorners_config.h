#pragma once

 
#include <kcmodule.h>

class KwinCornersConfig : public KCModule
{
    Q_OBJECT
public:
    explicit KwinCornersConfig(QWidget* parent = 0, const QVariantList& args = QVariantList());
    ~KwinCornersConfig();
public slots:
    void save();
    void load();
    void defaults();

private:
    class Private;
    Private * const d;
    friend class Private;
};