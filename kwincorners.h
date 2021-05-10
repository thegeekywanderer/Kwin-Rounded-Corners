#pragma once    
#include <kwineffects.h>

namespace KWin { class GLTexture; }

class Q_DECL_EXPORT KwinCornersEffect : public KWin::Effect {
    Q_OBJECT

    public:
        KwinCornersEffect();
        ~KwinCornersEffect();

        static bool supported();
        static bool enabledByDefault();

        void genMasks();
        void genRect();

        void fillRegion(const QRegion &reg, const QColor &c);

        void readConfig();
        void reconfigure(ReconfigureFlags flags);

        bool isValid(KWin::EffectWindow *window);
        void paintWindow(KWin::EffectWindow *window, int mask, QRegion region, KWin::WindowPaintData& data);
        int requestedEffectChainPosition() const { return 100; }

    private:
        enum {TopLeft = 0, TopRight, BottomRight, BottomLeft, NTex };
        KWin::GLTexture *m_tex[NTex];
        KWin::GLTexture *m_rect[NTex];

        QList<int> m_size;
        QList<QSize> m_corner;
        QRegion m_updateRegion;

        KWin::GLShader *m_shader;
        enum CornerType {
            Normal = 0,
            Rounded,
            Chiseled,
        };
        CornerType m_type = CornerType::Normal;

        int m_alpha, m_rSize;
        bool m_outline = true; 
        bool squareAtEdge = false;
        bool filterShadow = false;

        QStringList whitelist;
        QStringList blacklist;
};