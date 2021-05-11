#ifndef KWINCORNERS_H
#define KWINCORNERS_H

#include <kwineffects.h>

namespace KWin { class GLTexture; }

class Q_DECL_EXPORT KwinCornersEffect : public KWin::Effect
{
    Q_OBJECT
public:
    KwinCornersEffect();
    ~KwinCornersEffect();

    static bool supported();
    static bool enabledByDefault();

    void setRoundness(const int r);
    void genMasks();
    void genRect();

    void fillRegion(const QRegion &reg, const QColor &c);

    void reconfigure(ReconfigureFlags flags);
#if KWIN_EFFECT_API_VERSION >= 232
    void prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, std::chrono::milliseconds time);
#else
    void prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, int time);
#endif
    void paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data);
    virtual int requestedEffectChainPosition() const { return 99; }

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);

private:
    enum { TopLeft = 0, TopRight, BottomRight, BottomLeft, NTex };
    KWin::GLTexture *m_tex[NTex];
    KWin::GLTexture *m_rect[NTex];
    int m_size, m_rSize, m_alpha;
    bool m_outline;
    QSize m_corner;
    QRegion m_updateRegion;
    KWin::GLShader *m_shader;
    QList<KWin::EffectWindow *> m_managed;
};

#endif 
