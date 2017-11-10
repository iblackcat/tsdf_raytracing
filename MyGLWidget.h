#pragma once

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QOpenGLTexture>
#include <QVector>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

#include "MyArcball.h"
#include "utils.h"

class MyGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	MyGLWidget(QWidget *parent);
	~MyGLWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void wheelEvent(QWheelEvent *event);

private:
	const char* ReadShaderFile(const char* filepath);
	void drawTriangle();
	void setMatrix();

	QGLShaderProgram program;
	QOpenGLTexture *textureC;
	QOpenGLTexture *textureSW;
	mf::CameraPose pose;
	mf::double3 axis[6];

	QVector<QVector3D> vertices;
	QVector<QVector2D> vts;
	int mMVPMatrixHandle;
	int mPositionHandle;
	int mTexcoordHandle;
	int mAxisHandle;
	int mInvQHandle;
	int mqHandle;
	
	QMatrix4x4 mModelMatrix;//ƒ£–Õæÿ’Û
	QMatrix4x4 mViewMatrix; // ”Õºæÿ’Û  
	QMatrix4x4 mProjectionMatrix; //Õ∂”∞æÿ’Û  
	QMatrix4x4 mMVPMatrix; //MVPæÿ’Û

	MyArcball m_arcball;
	int frames;
	QTime time;
	QTimer *timer;
};
