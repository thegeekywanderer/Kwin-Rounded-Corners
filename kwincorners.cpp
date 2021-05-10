#include <QPainter>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <kwinglplatform.h>
#include <kwingutils.h>
#include <QMatrix4x4>
#include <KConfig>
#include <KConfigGroup>
#include "kwincorners.h"

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(
                                    KwinCornersFactory,
                                    KwinCornersEffect, 
                                    "kwincorners.json", 
                                    return KwinCornersEffect::supported();,
                                    return KwinCornersEffect::enabledbydefault();)

KwinCornersEffect::KwinCornersEffect() : Kwin::Effect(), m_shader(0)
{
    for (int i = 0; i < NTex; ++i){
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
    QList<int> cornerSizes = generalGroup.readEntry("Radius", QList<int>{10, 10, 10, 10});
    QString type = generalGroup.readEntry("Type", QString("rounded"));

    m_outline = generalGroup.readEntry("Outline", false);
    squareAtEdge = generalGroup.readEntry("SquareAtScreenEdge", false);
	filterShadow = generalGroup.readEntry("FilterShadow", false);

    m_alpha = int(generalGroup.readEntry("OutlineStrength", 15) * 2.55)

    whitelist = generalGroup.readEntry("Whitelist", QStringList());
	blacklist = generalGroup.readEntry("Blacklist", QStringList());

    if (type.contains(QString("rounded"), Qt::CaseInsensitive))
	{
		m_type = CornerType::Rounded;
	}
	else if (type.contains(QString("chiseled"), Qt::CaseInsensitive))
	{
		m_type = CornerType::Chiseled;
	}

    bool isAllZero = true;
    for (auto &size : cornerSizes)
	{
		if (size < 0)
			size = 0;
		isAllZero = isAllZero && (size == 0);
	}

	if (isAllZero)
	{
		m_type = CornerType::Normal;
	}
	else
	{
		switch (cornerSizes.size())
		{
		case 1: // All four corners have same radius
			cornerSizes.push_back(cornerSizes.at(0));
			cornerSizes.push_back(cornerSizes.at(0));
			cornerSizes.push_back(cornerSizes.at(0));
			break;
		case 2: // Top Left = Bottom Right and Top Right = Bottom Left
			cornerSizes.push_back(cornerSizes.at(0));
			cornerSizes.push_back(cornerSizes.at(1));
			break;
		case 3: // Top Left = Bottom Right
			cornerSizes.push_back(cornerSizes.at(1));
			break;
		}

		m_size = cornerSizes;
	}
}

KwinCornersEffect::genMasks() {
    for (int i = 0; i < NTex; ++i)
        if (m_tex[i])
            delete m_tex[i];
    
    if (m_type == CornerType::Normal)
		return;

    for (int i = 0; i < NTex; i++)
	{
		auto size = m_size.at(i);
		QImage img(size * 2, size * 2, QImage::Format_ARGB32_Premultiplied);
		img.fill(Qt::transparent);

		QPainter p(&img);
		p.fillRect(img.rect(), Qt::black);
		p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
		p.setPen(Qt::NoPen);
		p.setBrush(Qt::black);
		p.setRenderHint(QPainter::HighQualityAntialiasing);

		if (m_type == CornerType::Rounded)
		{
			p.drawEllipse(img.rect());
		}
		else if (m_type == CornerType::Chiseled)
		{
			const QPoint points[] = {
				QPoint(size, 0),
				QPoint(size * 2, size),
				QPoint(size, size * 2),
				QPoint(0, size)};

			p.drawPolygon(points, 4);
		}
		p.end();

		m_tex[i] = new KWin::GLTexture(img.copy(
			i == 1 || i == 2 ? size : 0,
			i == 0 || i == 1 ? size : 0,
			size, size));
	}
}

KwinCornersEffect::genRect()
{
    for (int i = 0; i < NTex; ++i)
        if (m_rect[i])
            delete m_rect[i];

    for (int i = 0; i < NTex; i++)
	{
		auto size = m_size.at(i);
        m_rSize = size + 1;
		QImage img(m_rSize * 2, m_rSize * 2, QImage::Format_ARGB32_Premultiplied);
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

		m_rect[i] = new KWin::GLTexture(img.copy(
			i == 1 || i == 2 ? m_rSize : 0,
			i == 0 || i == 1 ? m_rSize : 0,
			m_rSize, m_rSize));
	}
}

void KwinCornersEffect::reconfigure(ReconfigureFlags flags)
{
	Q_UNUSED(flags)

	readConfig();

	m_corner.clear();
	for (const auto size : m_size)
		m_corner.push_back(QSize(size, size));

	genMasks();
}