#include <cmath>
#include <QtWidgets>
#include "waveplotter.h"

WavePlotter::WavePlotter(QWidget *parent)
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::StrongFocus);

	setPlotSettings(PlotSettings());

}

WavePlotter::~WavePlotter()
{

}

void WavePlotter::setPlotSettings(const PlotSettings &settings)
{
	zoomStack.clear();
	zoomStack.append(settings);

	curZoom = 0;
	refreshPixmap();
}

void WavePlotter::setCurveData(int id, const QVector< QVector<QPointF>> &data)
{
	curveMap[id] = data;
	refreshPixmap();
}

void WavePlotter::clearCurve(int id)
{
	curveMap.remove(id);
	refreshPixmap();
}

int WavePlotter::getCurveNum()
{
	return curveMap.size();
}

QSize WavePlotter::minimumSizeHint() const
{
	return QSize(6*Margin, 4*Margin);
}

QSize WavePlotter::sizeHint() const
{
	return QSize(12*Margin, 8*Margin);
}

void WavePlotter::paintEvent(QPaintEvent *event)
{
	QStylePainter painter(this);
	painter.drawPixmap(0, 0, pixmap);

}

void WavePlotter::resizeEvent(QResizeEvent * /* event */)
{
	refreshPixmap();
}

void WavePlotter::refreshPixmap()
{
	pixmap = QPixmap(size());
	pixmap.fill(this,0,0);
	pixmap.fill(Qt::black);


	QPainter painter(&pixmap);
	painter.initFrom(this);
	drawGrid(&painter);
	drawCurves(&painter);
	update();
}

void WavePlotter::drawGrid(QPainter *painter)
{
	QRect rect(Margin,Margin,width() - 2*Margin, height()-2*Margin);
	if(!rect.isValid())
		return;

	PlotSettings settings = zoomStack[curZoom];

    QPen quiteDark = palette().dark().color().light();

	QPen grey(QColor(125,125,125));
//	QPen light = palette().light().color();
	QPen light = palette().midlight().color();

	for(int i = 0; i <= settings.numXTicks; ++i){
		int x = rect.left() + (i * (rect.width() -1)/settings.numXTicks);
		double label = settings.minX + (i * settings.spanX()
			/ settings.numXTicks);
		painter->setPen(grey);
		painter->drawLine(x, rect.top(), x, rect.bottom());
		painter->setPen(light);
		painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
		painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
			Qt::AlignHCenter | Qt::AlignTop,
			QString::number(label));
	}

	for (int j = 0; j <= settings.numYTicks; ++j) {
		int y = rect.bottom() - (j * (rect.height() - 1)
			/ settings.numYTicks);
		double label = settings.minY + (j * settings.spanY()
			/ settings.numYTicks);
		painter->setPen(grey);
		painter->drawLine(rect.left(), y, rect.right(), y);
		painter->setPen(light);
		painter->drawLine(rect.left() - 5, y, rect.left(), y);
		painter->drawText(rect.left() - Margin, y - 10, Margin - 5, 20,
			Qt::AlignRight | Qt::AlignVCenter,
			QString::number(label));
	}
	painter->drawRect(rect.adjusted(0, 0, -1, -1));
}


void WavePlotter::drawCurves(QPainter *painter)
{
	static const QColor colorForIds[6] = {
		Qt::white, Qt::red, Qt::green, Qt::blue, Qt::magenta, Qt::yellow
	};
	PlotSettings settings = zoomStack[curZoom];
	QRect rect(Margin, Margin,
		width() - 2 * Margin, height() - 2 * Margin);
	if (!rect.isValid())
		return;

	painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

	QMapIterator<int, QVector< QVector<QPointF>> > i(curveMap);
	while (i.hasNext()) {
		i.next();

		int id = i.key();
		QVector< QVector<QPointF>> dataseq = i.value();
		QVector<QPolygonF> polylineseq;
		for(size_t k = 0; k < dataseq.size(); ++k){
			QVector<QPointF> data = dataseq[k];
			QPolygonF polyline(data.count());

			for (int j = 0; j < data.count(); ++j) {
				double dx = data[j].x() - settings.minX;
				double dy = data[j].y() - settings.minY;
				double x = rect.left() + (dx * (rect.width() - 1)
					/ settings.spanX());
				double y = rect.bottom() - (dy * (rect.height() - 1)
					/ settings.spanY());
				polyline[j] = QPointF(x, y);
			}
			polylineseq.push_back(polyline);
		}

		painter->setPen(colorForIds[uint(id) % 6]);
		QPen curpen = painter->pen();
		if(0 == id){
			curpen.setWidth(2);
			painter->setPen(curpen);
			painter->drawPoints(polylineseq[0]);

		}else{
			for(size_t k = 0; k < polylineseq.size(); k++){
				painter->drawPolyline(polylineseq[k]);
			}		
		}
		
	}
}

PlotSettings::PlotSettings()
{
	minX = 0.0;
	maxX = 260.0;
	numXTicks = 26;

	minY = 0.0;
	maxY = 150;
	numYTicks = 15.0;
}
