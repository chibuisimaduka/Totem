﻿#include "../designnetbase/designnetspace.h"
#include "../data/matrixdata.h"
#include "../designnetbase/processorconfigmanager.h"
#include "../widgets/processorconfigwidget.h"
#include "processorgraphicsblock.h"
#include "property/property.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "GraphicsUI/graphicstoolbutton.h"
#include "GraphicsUI/textanimationblock.h"
#include "designnetconstants.h"
#include "graphicsitem/blocktextitem.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"


#include <QApplication>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsScene>
#include <QMenu>
#include <QFont>
#include <QTimer>
#include <QTime>
#include <QGraphicsBlurEffect>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
using namespace GraphicsUI;
namespace DesignNet{
const int PORT_MARGIN = 15;


void Position::serialize(Utils::XmlSerializer& s) const
{
	s.serialize("X", m_x);
	s.serialize("Y", m_y);
	s.serialize("id", m_id);
}

void Position::deserialize(Utils::XmlDeserializer& s) 
{
	s.deserialize("id", m_id);
	s.deserialize("X", m_x);
	s.deserialize("Y", m_y);
}

ProcessorGraphicsBlock::ProcessorGraphicsBlock( Processor *processor /*= 0*/, QGraphicsScene *s /*= 0*/, const QPointF &pos /*= QPointF(0, 0)*/, QGraphicsItem *parent /*= 0*/ )
	: QGraphicsObject(parent), m_processor(processor)
{
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);
	setCacheMode(DeviceCoordinateCache);
	/// title
	m_titleItem = new BlockTextItem(this);
	m_titleItem->setBlockName(m_processor->name());
	///
	/// close按钮
	m_closeItem = new GraphicsAutoShowHideItem(this);
	m_closeItem->setSize(QSize(16, 16));
	m_closeItem->setPos(boundingRect().right() - m_closeItem->boundingRect().width() - ITEMSPACING, boundingRect().top());
	m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
	QObject::connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));

	QObject::connect(this, SIGNAL(processorLog(QString)),
		Core::ICore::messageManager(),
		SLOT(printToOutputPanePopup(QString)));
	
	if (m_processor)
		QObject::connect(m_processor, SIGNAL(logout(QString)), this, SLOT(onShowLog(QString)));

	m_layoutDirty = true;
	
	init();

	QFont font("Time", 10);
	m_titleItem->setPos(boundingRect().left() + ITEMSPACING, boundingRect().top() + ITEMSPACING);
	m_titleItem->setMaxWidth(boundingRect().width() - m_closeItem->boundingRect().width() - (ITEMSPACING<<1));
	if (s)
	{
		s->addItem(this);
		this->setPos(pos);
	}
	m_configWidget = 0;
}

ProcessorGraphicsBlock::~ProcessorGraphicsBlock()
{
	if (m_configWidget)
	{
		m_configWidget->close();
		m_configWidget->deleteLater();
	}
}

QRectF ProcessorGraphicsBlock::boundingRect() const
{
	QSizeF size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	return QRectF(-size.width()/ 2, -size.height()/2, size.width(), size.height());
}

QSizeF ProcessorGraphicsBlock::minimumSizeHint() const
{
    return QSizeF(DEFAULT_WIDTH, TITLE_HEIGHT + 2 * PORT_MARGIN + 1);
}

void ProcessorGraphicsBlock::layoutItems()
{
    prepareGeometryChange();

	
}

Processor *ProcessorGraphicsBlock::processor()
{
    return (Processor *)m_processor;
}

void ProcessorGraphicsBlock::init()
{
	if (scene())
		this->setPos(QPointF(m_pos.m_x, m_pos.m_y));
}

void ProcessorGraphicsBlock::propertyRemoving(Property *prop)
{
}

void ProcessorGraphicsBlock::propertyRemoved(Property *prop)
{
}

QVariant ProcessorGraphicsBlock::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemSelectedHasChanged)
    {
        if(value.toBool() == false)
        {
            emit selectionChanged(false);
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

int ProcessorGraphicsBlock::type() const
{
    return ProcessorGraphicsBlock::Type;
}

void ProcessorGraphicsBlock::propertyAdded( Property* prop )
{
	QObject::connect(prop, SIGNAL(changed()), this, SLOT(onPropertyChanged_internal()));	
}

void ProcessorGraphicsBlock::onPropertyChanged_internal()
{
	Property *prop = qobject_cast<Property*>(sender());
	if(prop)
		propertyChanged(prop);
}

void ProcessorGraphicsBlock::propertyChanged( Property *prop )
{

}

void ProcessorGraphicsBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget* widget /*= 0 */ )
{
	prepareGeometryChange();
	
	QRectF rectF = boundingRect();
	painter->setPen(Qt::NoPen);
	QRectF rectShadow = rectF;
	rectShadow.translate(5, 5);
	painter->fillRect(rectShadow, qApp->palette().shadow().color());

	painter->fillRect(rectF, QColor(102, 175,253));
	if(isSelected())
	{
		painter->save();
		painter->setPen(QColor(255, 255, 255));
		QRectF rect = boundingRect();
		rect.adjust(2, 2, -2, -2);
		painter->drawRect(rect);

		painter->setPen(QPen(Qt::darkBlue, 2));
		rect = boundingRect();
		painter->drawRect(rect);
		painter->restore();
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
	}
	else
	{
		this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Normal);
	}
	painter->save();
	painter->setPen(QPen(Qt::white, 1));
	painter->drawLine(QPointF(rectF.left(), rectF.top() + TITLE_HEIGHT), QPointF(rectF.right(), rectF.top() + TITLE_HEIGHT));
	int index = m_processor->id();
	QFont font("Time", 10);
	QFontMetrics fm(font);
	QRectF indexRect = fm.boundingRect(tr("%1").arg(index));
	qreal tempWidth = qMax(indexRect.width(), indexRect.height());
	QRectF e(rectF.right() - tempWidth - ITEMSPACING, rectF.bottom() - tempWidth - ITEMSPACING, 
		tempWidth, tempWidth);
	painter->setBrush(Qt::black);
	painter->drawEllipse(e);
	painter->drawText(e, Qt::AlignCenter, tr("%1").arg(index));
	painter->setRenderHints(QPainter::Antialiasing);
	painter->restore();
}

void ProcessorGraphicsBlock::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->animateShow(true);
	QGraphicsItem::hoverEnterEvent(event);
}

void ProcessorGraphicsBlock::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	m_closeItem->animateShow(false);
	QGraphicsItem::hoverLeaveEvent(event);
}

void ProcessorGraphicsBlock::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void ProcessorGraphicsBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	this->update();
	QGraphicsItem::mouseMoveEvent(event);
}

void ProcessorGraphicsBlock::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
	QGraphicsItem::mouseReleaseEvent(event);
}

void ProcessorGraphicsBlock::serialize( Utils::XmlSerializer& s ) const
{
	m_processor->serialize(s);
	Position p(this->scenePos());
	s.serialize("Pos", p);
}

void ProcessorGraphicsBlock::deserialize( Utils::XmlDeserializer& s )
{
	s.deserialize("Pos", m_pos);
}

DesignNet::Position ProcessorGraphicsBlock::originalPosition() const
{
	return m_pos;
}

void ProcessorGraphicsBlock::createContextMenu( QMenu *parentMenu )
{

}

float ProcessorGraphicsBlock::statusBarHeight()
{
	return (float)STATUSBAR_HEIGHT;
}

void ProcessorGraphicsBlock::setLayoutDirty( const bool &dirty /*= true*/ )
{
	m_layoutDirty = dirty;
}

void ProcessorGraphicsBlock::setState( const State &s, const bool &bAdd /*= false*/ )
{
	if (bAdd)
	{
		m_state |= s;
	}
	else
		m_state = s;
}

void ProcessorGraphicsBlock::onShowLog( const QString &log )
{
	QString strTimeLog = QTime::currentTime().toString("hh:mm:ss.zzz");
	strTimeLog = strTimeLog + "> " + sender()->objectName() + ": " + log;
	emit processorLog(strTimeLog);
}

void ProcessorGraphicsBlock::mouseDoubleClickEvent(  QGraphicsSceneMouseEvent * event )
{
	if (!m_configWidget)
	{
		m_configWidget = ProcessorConfigManager::instance()->createConfigWidget(m_processor, 0);
		QObject::connect(m_configWidget, SIGNAL(destroyed ( QObject * )), this, SLOT(configWidgetClosed()));
	}
	if (m_configWidget)
	{
		m_configWidget->move(event->buttonDownScreenPos(Qt::LeftButton));
		m_configWidget->show();
		
		m_configWidget->activateWindow();
	}
}

void ProcessorGraphicsBlock::configWidgetClosed()
{
	m_configWidget = 0;
}

void ProcessorGraphicsBlock::startDeserialize(Utils::XmlDeserializer& s)
{
	deserialize(s);
}

void ProcessorGraphicsBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{
	event->accept();
}

}
