#ifndef __TIME_OF_DAY_FADER_H__
#define __TIME_OF_DAY_FADER_H__

#include "I3DEngine.h"
#include "ISituationListener.h"

struct SSituationTodVar
{
	int id;
	float fFadeInTime;
	float fFadeProgression;


	/** INTERNAL Used when tod fader fades out vars
	* upon a situation change. It's just a storage variable
	* which stores fFadeInTime during fading out as the fade out time depends
	* on the time left until a situation change and thus has to be saved inside fFadeInTime
	* Once the OnSituationChanging event occurs, fFadeInTime will be assigned with the original value from this var
	* Saves the trouble of reloading definitions and recoding large parts ;) */
	float fFadeInStorage;




	bool bFading;
	ITimeOfDay::EVariableType type;
	float pValues[3];

	SSituationTodVar() : id(0), fFadeInTime(1),
		fFadeProgression(0), bFading(true),
		type(ITimeOfDay::EVariableType::TYPE_FLOAT)
	{
		pValues[0] = 0;
		pValues[1] = 0;
		pValues[2] = 0;
	}
};

class CTimeOfDayFader : public ITimeOfDayUpdateCallback, IWeatherSystemListener
{
public:
	CTimeOfDayFader();
	virtual ~CTimeOfDayFader();

	// ITimeOfDayUpdateCallback
	virtual void BeginUpdate() override;
	virtual bool GetCustomValue(ITimeOfDay::ETimeOfDayParamID paramID, int dim, float *pValues, float &blendWeight) override;
	virtual void EndUpdate() override;

	// IWeatherSystemListener
	void OnSituationAboutToChange(float fTimeLeft) override;
	void OnSituationChanging(string sNextSituation) override;

	// CTimeOfDayFader
	bool Init();
	void Update(float fFrameTime);
	void LoadFromXml(XmlNodeRef root);
	void Reset();


	static bool GetIdFromReadableName(const char *sName, ITimeOfDay::ETimeOfDayParamID &out_paramId);

private:

	std::vector<SSituationTodVar> m_todVars;
	bool m_bIsActive;
	bool m_bFadeOut;

};

#endif // !__TIME_OF_DAY_FADER_H__
