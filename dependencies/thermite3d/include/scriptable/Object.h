#ifndef OBJECT_H_
#define OBJECT_H_

#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

namespace Thermite
{
	class Component;

	class Object : public QObject
	{
		Q_OBJECT

	public:
		Object();
		Object(Object* parent);
		Object(const QString& name);
		Object(const QString& name, Object* parent);
		~Object();

		Q_PROPERTY(QVector3D position READ position WRITE setPosition)
		Q_PROPERTY(QQuaternion orientation READ orientation WRITE setOrientation)
		Q_PROPERTY(QVector3D size READ size WRITE setSize)

		Q_PROPERTY(QVector3D derivedPosition READ derivedPosition)
		Q_PROPERTY(QQuaternion derivedOrientation READ derivedOrientation)
		Q_PROPERTY(QVector3D derivedSize READ derivedSize)
		Q_PROPERTY(QMatrix4x4 transform READ transform)

		Q_PROPERTY(QVector3D xAxis READ xAxis)
		Q_PROPERTY(QVector3D yAxis READ yAxis)
		Q_PROPERTY(QVector3D zAxis READ zAxis)

		Q_PROPERTY(QVector3D derivedXAxis READ derivedXAxis)
		Q_PROPERTY(QVector3D derivedYAxis READ derivedYAxis)
		Q_PROPERTY(QVector3D derivedZAxis READ derivedZAxis)

		const QVector3D& position(void) const;
		void setPosition(const QVector3D& position);

		const QQuaternion& orientation(void) const;
		void setOrientation(const QQuaternion& orientation);

		const QVector3D& size(void) const;
		void setSize(const QVector3D& size);

		const QVector3D derivedPosition(void) const;
		const QQuaternion derivedOrientation(void) const;
		const QVector3D derivedSize(void) const;
		const QMatrix4x4 transform(void) const;

		const QVector3D xAxis(void) const;
		const QVector3D yAxis(void) const;
		const QVector3D zAxis(void) const;

		const QVector3D derivedXAxis(void) const;
		const QVector3D derivedYAxis(void) const;
		const QVector3D derivedZAxis(void) const;

		void setComponent(Component* component);
		Component* getComponent(void);

	public slots:
		void translate(const QVector3D & vector);
		void translate(qreal x, qreal y, qreal z);

		void pitch(qreal angleInDegrees);
		void yaw(qreal angleInDegrees);
		void roll(qreal angleInDegrees);

		void scale(qreal factor);
		void scale(const QVector3D & vector);
		void scale(qreal x, qreal y, qreal z);

		void lookAt(const QVector3D& target);

	public:
		QVector3D mPosition;
		QQuaternion mOrientation;
		QVector3D mScale;

		Component* mComponent;
	};
}

#endif //OBJECT_H_