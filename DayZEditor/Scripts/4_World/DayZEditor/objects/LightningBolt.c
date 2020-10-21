

class InclementDabLightning: ScriptedLightBase
{    
    void InclementDabLightning()
    {
		SetLightType(LightSourceType.PointLight);
        SetVisibleDuringDaylight(true);
        SetRadius(1000);
        SetBrightness(0.75);
    }
}

private static const string BOLT_TYPES[2] = { "ThunderboltNorm", "ThunderboltHeavy" };
private static const string SOUND_TYPES[4] = { "ThunderNorm_Near_SoundSet", "ThunderNorm_Far_SoundSet", "ThunderHeavy_Near_SoundSet", "ThunderHeavy_Far_SoundSet"};


	