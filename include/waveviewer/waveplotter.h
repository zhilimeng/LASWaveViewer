#ifndef WAVEPLOTTER_H
#define WAVEPLOTTER_H

#include <QtWidgets>
#include <QMap>
#include <QPixmap>
#include <QVector>

class PlotSettings;
class WavePlotter : public QWidget
{
	Q_OBJECT

public:
	WavePlotter(QWidget *parent);
	~WavePlotter();

	void setPlotSettings(const PlotSettings &settings);
	void setCurveData(int id, const QVector< QVector<QPointF>> &data);
	void clearCurve(int id);
	int getCurveNum();
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	void refreshPixmap();
	void drawGrid(QPainter *painter);
	void drawCurves(QPainter *painter);


	QVector<PlotSettings> zoomStack;
	int curZoom;
	enum{ Margin = 50};
	QVector<QPointF> pointData;
	QMap<int, QVector< QVector<QPointF>> > curveMap;
	QPixmap pixmap;
	
};

class PlotSettings
{
public:
	PlotSettings();



	double spanX() const { return maxX - minX; }
	double spanY() const { return maxY - minY; }

	double minX;
	double maxX;
	int numXTicks;
	double minY;
	double maxY;
	int numYTicks;
};
#endif // WAVEPLOTTER_H
