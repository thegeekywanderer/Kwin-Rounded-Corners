#include <QPainter>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <kwinglplatform.h>
#include <kwinglutils.h>
#include <QMatrix4x4>
#include <KConfig>
#include <KConfigGroup>
#include "kwincorners.h"

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  KwinCornersFactory,
                                        KwinCornersEffect,
                                        "kwincorners.json",
                                        return KwinCornersEffect::supported();,
                                        return KwinCornersEffect::enabledByDefault();)

KwinCornersEffect::KwinCornersEffect() : KWin::Effect(), m_shader(0)
{
    for (int i = 0; i < NTex; ++i)
    {
        m_tex[i] = 0;
        m_rect[i] = 0;
    }
    reconfigure(ReconfigureAll);

    QString shadersDir(QStringLiteral("kwin/shaders/1.10/"));

    const qint64 version = KWin::kVersionNumber(1, 40);
    if (KWin::GLPlatform::instance()->glslVersion() >= version)
		shadersDir = QStringLiteral("kwin/shaders/1.40/");

    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("kwincorners.frag"));

    QFile file(fragmentshader);

    if (file.open(QFile::ReadOnly))
	{
		QByteArray frag = file.readAll();
		m_shader = KWin::ShaderManager::instance()->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
		file.close();

		if (m_shader->isValid())
		{
			const int sampler = m_shader->uniformLocation("sampler");
			const int corner = m_shader->uniformLocation("corner");
			KWin::ShaderManager::instance()->pushShader(m_shader);
			m_shader->setUniform(corner, 1);
			m_shader->setUniform(sampler, 0);
			KWin::ShaderManager::instance()->popShader();
		}
	}
	else
		deleteLater();
}

KwinCornersEffect::~KwinCornersEffect()
{
    if (m_shader)
        delete m_shader;
    for (int i = 0; i < NTex; ++i)
    {
        if (m_tex[i])
            delete m_tex[i];
        if (m_rect[i])
            delete m_rect[i];
    }
}

void KwinCornersEffect::readConfig() {

    KConfig config("kwincornersrc", KConfig::SimpleConfig);

    KConfigGroup generalGroup(&config, "General");
    m_size = int(generalGroup.readEntry("Radius", 10));

    m_outline = generalGroup.readEntry("Outline", false);
    squareAtEdge = generalGroup.readEntry("SquareAtScreenEdge", false);
	filterShadow = generalGroup.readEntry("FilterShadow", false);

    m_alpha = int(generalGroup.readEntry("OutlineStrength", 15) * 2.55);

    whitelist = generalGroup.readEntry("Whitelist", QStringList());
	blacklist = generalGroup.readEntry("Blacklist", QStringList());


}

void KwinCornersEffect::genMasks()
{
    for (int i = 0; i < NTex; ++i)
        if (m_tex[i])
            delete m_tex[i];


    QImage img(m_size*2, m_size*2, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p(&img);
    p.fillRect(img.rect(), Qt::black);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(img.rect());
    p.end();

    m_tex[TopLeft] = new KWin::GLTexture(img.copy(0, 0, m_size, m_size));
    m_tex[TopRight] = new KWin::GLTexture(img.copy(m_size, 0, m_size, m_size));
    m_tex[BottomRight] = new KWin::GLTexture(img.copy(m_size, m_size, m_size, m_size));
    m_tex[BottomLeft] = new KWin::GLTexture(img.copy(0, m_size, m_size, m_size));
    
}

void KwinCornersEffect::genRect()
{
    for (int i = 0; i < NTex; ++i)
        if (m_rect[i])
            delete m_rect[i];

    m_rSize = m_size+1;
    QImage img(m_rSize*2, m_rSize*2, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    QRect r(img.rect());
    p.setPen(Qt::NoPen);
    //p.setBrush(QColor(0, 0, 0, m_alpha));
    p.setRenderHint(QPainter::Antialiasing);
    //p.drawEllipse(r);
    //p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    //p.setBrush(Qt::black);
    r.adjust(1, 1, -1, -1);
    //p.drawEllipse(r);
    //p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(QColor(255, 255, 255, m_alpha));
    p.drawEllipse(r);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    r.adjust(1, 1, -1, -1);
    p.drawEllipse(r);
    p.end();

    m_rect[TopLeft] = new KWin::GLTexture(img.copy(0, 0, m_rSize, m_rSize));
    m_rect[TopRight] = new KWin::GLTexture(img.copy(m_rSize, 0, m_rSize, m_rSize));
    m_rect[BottomRight] = new KWin::GLTexture(img.copy(m_rSize, m_rSize, m_rSize, m_rSize));
    m_rect[BottomLeft] = new KWin::GLTexture(img.copy(0, m_rSize, m_rSize, m_rSize));
}

void KwinCornersEffect::reconfigure(ReconfigureFlags flags)
{
	Q_UNUSED(flags)

	readConfig();

	m_corner = QSize(m_size, m_size);

	genMasks();
    genRect();
}

bool KwinCornersEffect::isValid(KWin::EffectWindow *w)
{
	if (!m_shader->isValid())
		return false;

	auto className = w->windowClass();

	for (const auto &i : whitelist)
	{
		if (className.contains(i, Qt::CaseInsensitive))
			return true;
	}

	for (const auto &i : blacklist)
	{
		if (className.contains(i, Qt::CaseInsensitive))
			return false;
	}

	if (className.contains("plasma", Qt::CaseInsensitive) && !w->isNormalWindow() && !w->isDialog() && !w->isModal())
		return false;

	if (!w->isPaintingEnabled() || (w->isDesktop()) || w->isPopupMenu())
		return false;

	return true;
}

void
#if KWIN_EFFECT_API_VERSION >= 232
KwinCornersEffect::prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
#else
KwinCornersEffect::prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData &data, int time)
#endif
{
    if(!isValid(w))
	{
		KWin::effects->prePaintWindow(w, data, time);
        return;
	}   

    const QRect geo(w->geometry());
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft(), m_corner),
        QRect(geo.topRight()-QPoint(m_size-1, 0), m_corner),
        QRect(geo.bottomRight()-QPoint(m_size-1, m_size-1), m_corner),
        QRect(geo.bottomLeft()-QPoint(0, m_size-1), m_corner)
    };
    for (int i = 0; i < NTex; ++i)
    {
        data.paint += rect[i];
        data.clip -= rect[i];
    }
    QRegion outerRect(QRegion(geo.adjusted(-1, -1, 1, 1))-geo.adjusted(1, 1, -1, -1));
    //outerRect += QRegion(geo.x()+m_size, geo.y(), geo.width()-m_size*2, 1);
    data.paint += outerRect;
    data.clip -=outerRect;
    KWin::effects->prePaintWindow(w, data, time);
}

void KwinCornersEffect::paintWindow(KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data)
{
    KWin::WindowQuadList qds(data.quads);

	if (filterShadow)
		data.quads = qds.filterOut(KWin::WindowQuadShadow);


	if(!isValid(w))
	{
		KWin::effects->paintWindow(w, mask, region, data);
		return;
	}

    // Map the corners
    const QRect geo(w->geometry());
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft(), m_corner),
        QRect(geo.topRight()-QPoint(m_size-1, 0), m_corner),
        QRect(geo.bottomRight()-QPoint(m_size-1, m_size-1), m_corner),
        QRect(geo.bottomLeft()-QPoint(0, m_size-1), m_corner)
    };

    // Copy the corner regions
	KWin::GLTexture tex[NTex];
	const QRect s(KWin::effects->virtualScreenGeometry());
	for (int i = 0; i < NTex; ++i)
	{
		tex[i] = KWin::GLTexture(GL_RGBA8, rect[i].size());
		tex[i].bind();
		glCopyTexSubImage2D(
			GL_TEXTURE_2D, 0, 0, 0,
			rect[i].x(), 
			s.height() - rect[i].y() - rect[i].height(),
			rect[i].width(), 
			rect[i].height());
		tex[i].unbind();
	}

    // Paint the window content
    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const int mvpMatrixLocation = m_shader->uniformLocation("modelViewProjectionMatrix");
    
    KWin::ShaderManager *sm = KWin::ShaderManager::instance();
    sm->pushShader(m_shader);

    // bool cornerConditions[] = {
	// 	squareAtEdge && (geo.left() == 0 || geo.top() == 0),
	// 	squareAtEdge && ((geo.right() + 1) == s.width() || geo.top() == 0),
	// 	squareAtEdge && ((geo.right() + 1) == s.width() || (geo.bottom() + 1) == s.height()),
	// 	squareAtEdge && (geo.left() == 0 || (geo.bottom() + 1) == s.height())};

    for (int i = 0; i < NTex; ++i)
    {
        // if (cornerConditions[i])
		// 	continue;

        QMatrix4x4 mvp = data.screenProjectionMatrix();
        mvp.translate(rect[i].x(), rect[i].y());
        m_shader->setUniform(mvpMatrixLocation, mvp);
        glActiveTexture(GL_TEXTURE1);
        m_tex[3-i]->bind();
        glActiveTexture(GL_TEXTURE0);
        tex[i].bind();
        tex[i].render(region, rect[i]);
        tex[i].unbind();
        m_tex[3-i]->unbind();
    }

    sm->popShader();
    data.quads = qds;

    // outline
    if (m_outline && data.brightness() == 1.0 && data.crossFadeProgress() == 1.0)
    {
        const QRect rrect[NTex] =
        {
            rect[0].adjusted(-1, -1, 0, 0),
            rect[1].adjusted(0, -1, 1, 0),
            rect[2].adjusted(0, 0, 1, 1),
            rect[3].adjusted(-1, 0, 0, 1)
        };
        const float o(data.opacity());
        KWin::GLShader *shader = KWin::ShaderManager::instance()->pushShader(KWin::ShaderTrait::MapTexture|KWin::ShaderTrait::UniformColor|KWin::ShaderTrait::Modulate);
        shader->setUniform(KWin::GLShader::ModulationConstant, QVector4D(o, o, o, o));
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        for (int i = 0; i < NTex; ++i)
        {
            QMatrix4x4 modelViewProjection;
            modelViewProjection.ortho(0, s.width(), s.height(), 0, 0, 65535);
            modelViewProjection.translate(rrect[i].x(), rrect[i].y());
            shader->setUniform("modelViewProjectionMatrix", modelViewProjection);
            m_rect[i]->bind();
            m_rect[i]->render(region, rrect[i]);
            m_rect[i]->unbind();
        }
        KWin::ShaderManager::instance()->popShader();
        
        shader = KWin::ShaderManager::instance()->pushShader(KWin::ShaderTrait::UniformColor);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        QRegion reg = geo;
        reg -= QRegion(geo.adjusted(1, 1, -1, -1));
        for (int i = 0; i < 4; ++i)
            reg -= rrect[i];
        fillRegion(reg, QColor(255, 255, 255, m_alpha*data.opacity()));
        
        KWin::ShaderManager::instance()->popShader();
    }

    glDisable(GL_BLEND);
}

void KwinCornersEffect::fillRegion(const QRegion &reg, const QColor &c)
{
    KWin::GLVertexBuffer *vbo = KWin::GLVertexBuffer::streamingBuffer();
    vbo->reset();
    vbo->setUseColor(true);
    vbo->setColor(c);
    QVector<float> verts;
    foreach (const QRect & r, reg.rects())
    {
        verts << r.x() + r.width() << r.y();
        verts << r.x() << r.y();
        verts << r.x() << r.y() + r.height();
        verts << r.x() << r.y() + r.height();
        verts << r.x() + r.width() << r.y() + r.height();
        verts << r.x() + r.width() << r.y();
    }
    vbo->setData(verts.count() / 2, 2, verts.data(), NULL);
    vbo->render(GL_TRIANGLES);
}

bool KwinCornersEffect::enabledByDefault()
{
    return supported();
}

bool KwinCornersEffect::supported()
{
    return KWin::effects->isOpenGLCompositing() && KWin::GLRenderTarget::supported();
}

#include "kwincorners.moc"