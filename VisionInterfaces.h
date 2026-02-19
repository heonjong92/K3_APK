#pragma once

class CVisionSystem;

namespace VisionInterface
{

class FrameGrabber
{
private:
	// do not copy
	const FrameGrabber& operator = ( const FrameGrabber& other );
	FrameGrabber( const FrameGrabber& other );

protected:
	CVisionSystem* m_pSystem;

public:
	FrameGrabber( CVisionSystem* pSystem ) : m_pSystem(pSystem) {}
	void Live( CameraType camType, int nGrabLength=0 );	// nGrabLength: linescan only
	void Grab( CameraType camType, int nGrabLength=0 ); // nGrabLength: linescan only
	void Idle( CameraType camType );
	BOOL IsLive( CameraType camType );
	BOOL IsOpen( CameraType camType );
	void ChangeExposure(CameraType camType, double dExposure);
	void SetPageCount( CameraType camType );
};

class LightController
{
private:
	// do not copy
	const LightController& operator = ( const LightController& other );
	LightController( const LightController& other );

protected:
	CVisionSystem* m_pSystem;
public:
	LightController( CVisionSystem* pSystem ) : m_pSystem(pSystem) {}
	BOOL TurnOn( LedCtrlType type, int nChannel, int nBrightness );
	BOOL TurnOff( LedCtrlType type, int nChannel );
	BOOL IsConnected( LedCtrlType type );
	BOOL ChangePort( LedCtrlType type, int nPort );
	BOOL ClosePort( LedCtrlType type );
};

} // namespace VisionInterface