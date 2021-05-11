#ifndef KWINCORNERS_ADAPTOR_H
#define KWINCORNERS_ADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>
#include "kwincorners.h"
#include <KConfigGroup>

namespace KWin
{
class EffectAdaptor : public QDBusAbstractAdaptor
{
   Q_OBJECT
   Q_CLASSINFO("D-Bus Interface", "org.kde.kwin")

private:
    KwinCornersEffect *m_effect;

public:
    EffectAdaptor(KwinCornersEffect *effect) : QDBusAbstractAdaptor(effect), m_effect(effect) { }

public slots:
    Q_NOREPLY void setRoundness(int r)
    {
        KConfigGroup conf = KSharedConfig::openConfig("kwincornersrc")->group("General");
        if (conf.readEntry("dsp", false))
        {
            conf.writeEntry("roundness", r);
            conf.sync();
            m_effect->setRoundness(r);
        }
    }
    Q_NOREPLY void configure() { m_effect->reconfigure(KWin::Effect::ReconfigureAll); }
};
}
#endif 