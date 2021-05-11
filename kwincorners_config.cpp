#include "kwincorners_config.h"
#include "ui_kwincorners_config.h"

#include <QDialog>
#include <QVBoxLayout>

#include <kwineffects.h>
#include <kwineffects_interface.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusArgument>

#include <KConfigGroup>
#include <KPluginFactory>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(KwinCornersConfigFactory,
                           "kwincorners_config.json",
                           registerPlugin<KwinCornersConfig>();)


class ConfigDialog : public QWidget , public Ui::Form
{
public:
    explicit ConfigDialog(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}
};

class KwinCornersConfig::Private
{
public:
    Private(KwinCornersConfig *config)
        : q(config)
        , roundness("roundness")
        , outline("outline")
        , alpha("alpha")
        , defaultRoundness(6)
        , defaultOutline(false)
        , defaultAlpha(12)
    {}
    KwinCornersConfig *q;
    QString roundness, outline, alpha;
    QVariant defaultRoundness, defaultOutline, defaultAlpha;
    ConfigDialog *ui;
};

KwinCornersConfig::KwinCornersConfig(QWidget* parent, const QVariantList& args)
    : KCModule(KAboutData::pluginData(QStringLiteral("kwin4_effect_kwincorners")), parent, args)
    , d(new Private(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(d->ui = new ConfigDialog(this));
    setLayout(layout);
}

KwinCornersConfig::~KwinCornersConfig()
{
    delete d;
}

void
KwinCornersConfig::load()
{
    KCModule::load();
    KConfigGroup conf = KSharedConfig::openConfig("kwincornersrc")->group("General");
    d->ui->roundness->setValue(conf.readEntry(d->roundness, d->defaultRoundness).toInt());
    d->ui->outline->setChecked(conf.readEntry(d->outline, d->defaultOutline).toBool());
    d->ui->alpha->setValue(conf.readEntry(d->alpha, d->defaultAlpha).toInt());
    emit changed(false);
}

void
KwinCornersConfig::save()
{
    KCModule::save();
    KConfigGroup conf = KSharedConfig::openConfig("kwincornersrc")->group("General");
    conf.writeEntry(d->roundness, d->ui->roundness->value());
    conf.writeEntry(d->outline, d->ui->outline->isChecked());
    conf.writeEntry(d->alpha, d->ui->alpha->value());
    conf.sync();
    emit changed(false);
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_kwincorners"));
}

void
KwinCornersConfig::defaults()
{
    KCModule::defaults();
    d->ui->roundness->setValue(d->defaultRoundness.toInt());
    d->ui->outline->setChecked(d->defaultOutline.toBool());
    d->ui->alpha->setValue(d->defaultAlpha.toInt());
    emit changed(true);
}

#include "kwincorners_config.moc"