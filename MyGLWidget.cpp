#include "MyGLWidget.h"

using namespace mf;

MyGLWidget::MyGLWidget(QWidget *parent)
	: QGLWidget(parent)
	, m_arcball(640, 480)
	, textureSW(0)
	, textureC(0)
{
	frames = 0;
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoBufferSwap(false);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(10);

	Matrix33d A(700, 0.0, 313, 0.0, 700.0, 256.0, 0.0, 0.0, 1.0);
	pose = CameraPose(A, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	axis[0] = double3(1.0, 0.0, 0.0);
	axis[1] = double3(-1.0, 0.0, 0.0);
	axis[2] = double3(0.0, 1.0, 0.0);
	axis[3] = double3(0.0, -1.0, 0.0);
	axis[4] = double3(0.0, 0.0, 1.0);
	axis[5] = double3(0.0, 0.0, -1.0);
}

MyGLWidget::~MyGLWidget()
{
	if (textureC)
		delete textureC;
	if (textureSW)
		delete textureSW;
}

const char* MyGLWidget::ReadShaderFile(const char* filepath) {
	FILE *fp = fopen(filepath, "rb");
	if (!fp) return NULL;
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *content = (char*)malloc(size + 1);
	fread(content, sizeof(char), size, fp);
	content[size] = '\0';
	fclose(fp);
	return (const char*)content;
}

void MyGLWidget::initializeGL() {
	QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
	const char *vsrc = ReadShaderFile("shader/default.vert");
	vshader->compileSourceCode(vsrc);

	QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
	const char *fsrc = ReadShaderFile("shader/default.frag");
	fshader->compileSourceCode(fsrc);

	program.addShader(vshader);
	program.addShader(fshader);
	program.link();
	
	mMVPMatrixHandle = program.uniformLocation("u_MVPMatrix");
	mPositionHandle = program.attributeLocation("p");
	mTexcoordHandle = program.attributeLocation("texCoord");
	mAxisHandle = program.attributeLocation("Axis");
	mInvQHandle = program.attributeLocation("Q");
	//mqHandle = program.attributeLocation("q");

	vertices << QVector3D(-1.0f, 1.0f, 0.0f)
			 << QVector3D( 1.0f, 1.0f, 0.0f)
			 << QVector3D(-1.0f,-1.0f, 0.0f)
			 << QVector3D( 1.0f, 1.0f, 0.0f)
			 << QVector3D( 1.0f,-1.0f, 0.0f)
			 << QVector3D(-1.0f,-1.0f, 0.0f);

	vts << QVector2D(0.0f, 1.0f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(0.0f, 0.0f)
		<< QVector2D(1.0f, 1.0f)
		<< QVector2D(1.0f, 0.0f)
		<< QVector2D(0.0f, 0.0f);

	mViewMatrix.lookAt(QVector3D(0.0f, 0.0f, 3.f), QVector3D(0.0f, 0.0f, -5.0f), QVector3D(0.0f, 1.0f, 0.0f));

	textureC = new QOpenGLTexture(QImage("res/modelC10.png"));
	textureC->setMinificationFilter(QOpenGLTexture::Linear);
	textureC->setMagnificationFilter(QOpenGLTexture::Linear);
	textureC->setWrapMode(QOpenGLTexture::Repeat);

	textureSW = new QOpenGLTexture(QImage("res/modelSW10.png"));
	textureSW->setMinificationFilter(QOpenGLTexture::Linear);
	textureSW->setMagnificationFilter(QOpenGLTexture::Linear);
	textureSW->setWrapMode(QOpenGLTexture::Repeat);
}

void MyGLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);

	float ratio = (float)w / h;
	float left = -1.0;
	float right = 1.0;
	float bottom = -1.0f;
	float top = 1.0f;
	float n = 0.999f;
	float f = 100.0f;

	mProjectionMatrix.frustum(left, right, bottom, top, n, f);
}

void MyGLWidget::drawTriangle() {
	program.enableAttributeArray(mPositionHandle);
	program.setAttributeArray(mPositionHandle, vertices.constData());
	program.enableAttributeArray(mTexcoordHandle);
	program.setAttributeArray(mTexcoordHandle, vts.constData());

	program.setUniformValue(mMVPMatrixHandle, mMVPMatrix);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	program.disableAttributeArray(mPositionHandle);
	program.disableAttributeArray(mTexcoordHandle);
}

void MyGLWidget::setMatrix() {
	///rotate left_buttons
	mModelMatrix.setToIdentity();
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			mModelMatrix(j, i) = m_arcball.ThisRot.M[i * 3 + j];
		}
	}

	///scale middle_button
	QVector3D pos = QVector3D(0.0f, 0.0f, m_arcball.mRadius);
	QVector3D target = QVector3D(0.0f, 0.0f, -5.0f);
	QVector3D up = QVector3D(0.0f, 1.0f, 0.0f);
	//pos = target + (pos - target)*m_arcball.mRadius;
	mViewMatrix.setToIdentity();
	mViewMatrix.lookAt(pos, target, up);

	///translate right_button
	mModelMatrix.translate(m_arcball.translate_x / 10.0, m_arcball.translate_y / 10.0, 0.0f);

	mMVPMatrix = mViewMatrix * mModelMatrix;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			pose.rot[i][j] = mMVPMatrix(j, i);
		}
	}
	mMVPMatrix = mProjectionMatrix * mViewMatrix * mModelMatrix;
	pose.rot = pose.rot.inv();
	//pose.rotation = pose.getRotation(pose.rot);
	//pose.rotation.print("rotation");
	//pose.rotation[2] *= -1;
	//pose.rotation[0] *= -1;
	//pose.rotation[1] *= -1;
	//pose.setMatrix();
	//pose.rot = pose.rot.inv();

	pose.translation[0] = -m_arcball.translate_x / 10;
	pose.translation[1] = -m_arcball.translate_y / 10;
	pose.translation[2] = 2.0 + m_arcball.mRadius;

	pose.refreshByARt();
	//Matrix33d A(700, 0.0, 313, 0.0, 700.0, 256.0, 0.0, 0.0, 1.0);
	//pose = CameraPose(A, 0, 0, 1, 0.0, 0, 5);

	float tmp = 0.0;
	int axis_tmp = 0;
	for (int i = 0; i < 6; i++) {
		if (pose.rot[2] * axis[i] > tmp) {
			tmp = pose.rot[2] * axis[i];
			axis_tmp = i;
		}
	}
	printf("axis_tmp: %d\n", axis_tmp);
	Matrix33d invQ = pose.Q.inv();
	QMatrix4x4 iQ;
	QVector3D q;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			iQ(i, j) = invQ[j][i];
		}
		iQ(i, 3) = pose.q[i];
		//q[i] = pose.q[i];
	}
	iQ(3, 3) = 0.0;

	/*
	pose.rot.print("rot");
	pose.translation.print("trans");
	pose.rotation = pose.getRotation(pose.rot);
	pose.rotation.print("rot");
	
	pose.q.print("q");
	*/
	program.setUniformValue("Q", iQ);
	program.setUniformValue("Axis", GLint(axis_tmp));
	program.setUniformValue("flag", GLint(0));
	program.setUniformValue("size", GLfloat(4.0));
	program.setUniformValue("Mu", GLint(8));
	//program.setUniformValue(mqHandle, q);

}

void MyGLWidget::paintGL()
{
	QPainter painter;
	painter.begin(this);

	painter.beginNativePainting();
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	program.bind(); //glUseProgram  
	setMatrix();

	program.setUniformValue("modelC", 0);
	textureC->bind();
	program.setUniformValue("modelSW", 1);
	textureSW->bind(1);



	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawTriangle();


	program.release();//glUseProgram(0)  
	painter.endNativePainting();


	// Do a complete rotation every 10 seconds.  
	long t = time.elapsed() % 10000L;
	float angleInDegrees = (360.0f / 10000.0f) * ((int)t);

	QString framesPerSecond;
	framesPerSecond.setNum(frames / (time.elapsed() / 1000.0), 'f', 2);

	QString mRadiusData;
	mRadiusData.setNum(m_arcball.mRadius);

	painter.setPen(Qt::white);

	//painter.drawText(20, 40, framesPerSecond + " fps");
	painter.drawText(20, 20, "mRadius: " + mRadiusData);

	painter.end();

	if (!(frames % 1000)) {
		time.start();
		frames = 0;
	}
	
	swapBuffers();
	frames++;
}

void MyGLWidget::mousePressEvent(QMouseEvent*mouse_event)
{
	QPoint qpoint = mapFromGlobal(QCursor::pos());
	int posx = qpoint.x();
	int posy = qpoint.y();
	if (mouse_event->button() == Qt::LeftButton)
	{
		m_arcball.MousePt.s.X = posx;
		m_arcball.MousePt.s.Y = posy;

		m_arcball.LastRot = m_arcball.ThisRot;
		m_arcball.ArcBall.click(&m_arcball.MousePt);
		m_arcball.button_status = 1;
	}
	else if (mouse_event->button() == Qt::RightButton)
		m_arcball.button_status = 2;
	m_arcball.mLastMousePos.s.X = static_cast<LONG>(qpoint.x());
	m_arcball.mLastMousePos.s.Y = static_cast<LONG>(qpoint.y());
	setMouseTracking(true);
	grabMouse();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *mouse_event)
{
	QPoint qpoint = mapFromGlobal(QCursor::pos());
	int posx = qpoint.x();
	int posy = qpoint.y();
	if (m_arcball.button_status == 1)
	{
		m_arcball.MousePt.s.X = posx;
		m_arcball.MousePt.s.Y = posy;
		Quat4fT     ThisQuat;
		m_arcball.ArcBall.drag(&m_arcball.MousePt, &ThisQuat);
		Matrix3fSetRotationFromQuat4f(&m_arcball.ThisRot, &ThisQuat);
		Matrix3fMulMatrix3f(&m_arcball.ThisRot, &m_arcball.LastRot);
		Matrix4fSetRotationFromMatrix3f(&m_arcball.Transform, &m_arcball.ThisRot);
	}
	else if (m_arcball.button_status == 2)
	{
		float dx = 0.1f*static_cast<float>(qpoint.x() - m_arcball.mLastMousePos.s.X);
		float dy = 0.1f*static_cast<float>(qpoint.y() - m_arcball.mLastMousePos.s.Y);
		m_arcball.translate_x += dx;
		m_arcball.translate_y -= dy;
	}
	m_arcball.mLastMousePos.s.X = posx;
	m_arcball.mLastMousePos.s.Y = posy;
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *mouse_event)
{
	releaseMouse();
	setMouseTracking(false);
}

void MyGLWidget::wheelEvent(QWheelEvent *event)
{
	float delta = (float)event->delta() / 1500.0f;
	m_arcball.mRadius *= (1 - delta);
}