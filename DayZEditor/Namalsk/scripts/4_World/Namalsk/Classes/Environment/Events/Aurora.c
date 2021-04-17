class Aurora extends EventBase
{
	private const int M_AURORA_CNT_LONG = 20, M_AURORA_CNT_SHORT = 43;
	private const int M_AUTORA_CNT_TOTAL = 63;

	protected vector m_AuroraParts[M_AUTORA_CNT_TOTAL];
	protected float m_AuroraParts_rot[M_AUTORA_CNT_TOTAL];
	protected float m_AuroraAltitude;

	protected autoptr array<EntityAI> m_AuroraInstances = new array<EntityAI>;

	private vector m_AuroraOffset;
	private vector m_AuroraOffsetLast;
	private vector m_AuroraOffsetDir;
	private float m_AuroraOffsetDelta;

	override void EventInit()
	{
		super.EventInit();

		m_EventID = 0;

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 60.0;
			m_MidPhaseLength = Math.RandomFloat(1600, 2000);
			m_EndPhaseLength = m_InitPhaseLength;
			m_NeededOvercast = 0.15;
		}

		if (GetGame().IsClient())
		{
			m_AuroraAltitude = 1250.0;
			m_AuroraOffset = Vector(0,0,0);
			m_AuroraOffsetLast = Vector(0,0,0);
			m_AuroraOffsetDir = Vector(0,0,0);
			m_AuroraOffsetDelta = 1.0;
		}
	}

	override bool EventActivateCondition()
	{
		if (m_wObject.GetOvercast().GetActual() <= m_NeededOvercast) {
			return true;
		} else {
			Print("[Namalsk][Aurora] failed to activate (needed ov: " + m_NeededOvercast + ", actual ov: " + m_wObject.GetOvercast().GetActual() + ")");
			return false;
		}
	}

	override void InitPhaseClient()
	{
		m_ClientTickRequired = -1.0;
		SpawnAuroraSlices();
	}
	override void InitPhaseServer()
	{
		Print("[Namalsk][Aurora] init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec)");

		// get weather state
		m_ovFore = m_wObject.GetOvercast().GetForecast();
		m_wObject.GetOvercast().GetLimits(m_ovMin, m_ovMax);

		// set weather state
		m_wObject.GetOvercast().SetLimits(0.0, 1.0);
		m_wObject.GetOvercast().Set(0.0, m_InitPhaseLength, m_MidPhaseLength);
	}
	override void InitPhaseServerLate() {}

	override void MidPhaseClient()
	{
		m_ClientTickRequired = 0.1;

		if (m_AuroraOffsetDelta > 1.0)
		{
			// choose a new direction
			m_AuroraOffsetLast = m_AuroraOffset;
			m_AuroraOffsetDir[0] = Math.RandomFloat(-60, 60);
			m_AuroraOffsetDir[2] = Math.RandomFloat(-60, 60);
			m_AuroraOffsetDelta = 0.0;
		} else {
			m_AuroraOffset[0] = Math.Lerp(m_AuroraOffsetLast[0], m_AuroraOffsetLast[0] + m_AuroraOffsetDir[0], m_AuroraOffsetDelta);
			m_AuroraOffset[2] = Math.Lerp(m_AuroraOffsetLast[2], m_AuroraOffsetLast[2] + m_AuroraOffsetDir[2], m_AuroraOffsetDelta);
			m_AuroraOffsetDelta += 0.01;
		}

		UpdateAuroraSlices();
	}
	override void MidPhaseServer() {}
	override void MidPhaseServerLate() {}

	override void EndPhaseClient()
	{
		m_ClientTickRequired = -1.0;
		RemoveAuroraSlices();
	}
	override void EndPhaseServer()
	{
		m_wObject.GetOvercast().Set(m_ovFore, m_EndPhaseLength, 0);
	}
	override void EndPhaseServerLate()
	{
		m_wObject.GetOvercast().SetLimits(m_ovMin, m_ovMax);
	}
	override void OnEventEndServer() {}

	// event-specific methods
	protected void SpawnAuroraSlices()
	{
		m_AuroraParts[0] = Vector(519.616287, 0.0, 1475.270316); m_AuroraParts_rot[0] = 260.796356;
		m_AuroraParts[1] = Vector(858.475373, 0.0, 1317.271053); m_AuroraParts_rot[1] = 331.158447;
		m_AuroraParts[2] = Vector(202.30951, 0.0, 786.391722); m_AuroraParts_rot[2] = 264.252289;
		m_AuroraParts[3] = Vector(1284.474539, 0.0, 647.541438); m_AuroraParts_rot[3] = 273.603699;
		m_AuroraParts[4] = Vector(1337.30448, 0.0, 302.403341); m_AuroraParts_rot[4] = 279.395905;
		m_AuroraParts[5] = Vector(1914.478187, 0.0, 409.938942); m_AuroraParts_rot[5] = 280.386932;
		m_AuroraParts[6] = Vector(1398.246379, 0.0, -164.207127); m_AuroraParts_rot[6] = 282.469147;
		m_AuroraParts[7] = Vector(1733.077033, 0.0, 65.199135); m_AuroraParts_rot[7] = 323.087738;
		m_AuroraParts[8] = Vector(569.332865, 0.0, -883.733999); m_AuroraParts_rot[8] = 252.905518;
		m_AuroraParts[9] = Vector(1831.389474, 0.0, -782.80959); m_AuroraParts_rot[9] = 324.526672;
		m_AuroraParts[10] = Vector(146.497852, 0.0, -1828.951313); m_AuroraParts_rot[10] = 318.880066;
		m_AuroraParts[11] = Vector(-1364.082566, 0.0, 1300.704661); m_AuroraParts_rot[11] = 286.636627;
		m_AuroraParts[12] = Vector(-174.2797, 0.0, 1396.955742); m_AuroraParts_rot[12] = 285.920715;
		m_AuroraParts[13] = Vector(-1439.000027, 0.0, 202.937454); m_AuroraParts_rot[13] = 337.188171;
		m_AuroraParts[14] = Vector(-798.194056, 0.0, 161.107969); m_AuroraParts_rot[14] = 332.282227;
		m_AuroraParts[15] = Vector(-906.330635, 0.0, -275.777575); m_AuroraParts_rot[15] = 280.838989;
		m_AuroraParts[16] = Vector(-277.415009, 0.0, -636.306379); m_AuroraParts_rot[16] = 322.347839;
		m_AuroraParts[17] = Vector(-1324.893914, 0.0, -815.801621); m_AuroraParts_rot[17] = 275.165802;
		m_AuroraParts[18] = Vector(-682.459291, 0.0, -1134.549398); m_AuroraParts_rot[18] = 316.724854;
		m_AuroraParts[19] = Vector(-2020.308841, 0.0, 774.968789); m_AuroraParts_rot[19] = 280.838989;

		m_AuroraParts[20] = Vector(358.978175, 0.0, 919.269615);  m_AuroraParts_rot[20] = 327.924927;
		m_AuroraParts[21] = Vector(949.907744, 0.0, 978.33795);  m_AuroraParts_rot[21] = 17.642456;
		m_AuroraParts[22] = Vector(542.841571, 0.0, 725.961456);  m_AuroraParts_rot[22] =309.369812;
		m_AuroraParts[23] = Vector(968.317582, 0.0, 751.725271);  m_AuroraParts_rot[23] =322.028168;
		m_AuroraParts[24] = Vector(655.00147, 0.0, 527.218389);  m_AuroraParts_rot[24] =354.957367;
		m_AuroraParts[25] = Vector(761.328473, 0.0, 343.435693);  m_AuroraParts_rot[25] =307.85614;
		m_AuroraParts[26] = Vector(960.337957, 0.0, 299.0952);  m_AuroraParts_rot[26] =253.896347;
		m_AuroraParts[27] = Vector(1028.521086, 0.0, -144.239094);  m_AuroraParts_rot[27] =77.185883;
		m_AuroraParts[28] = Vector(1602.557988, 0.0, 556.02717);  m_AuroraParts_rot[28] =322.028168;
		m_AuroraParts[29] = Vector(1995.16787, 0.0, -172.468047);  m_AuroraParts_rot[29] =287.2117;
		m_AuroraParts[30] = Vector(2220.593756, 0.0, -147.981098);  m_AuroraParts_rot[30] =239.099213;
		m_AuroraParts[31] = Vector(544.812641, 0.0, -307.198834);  m_AuroraParts_rot[31] =86.208923;
		m_AuroraParts[32] = Vector(789.39971, 0.0, -238.446932);  m_AuroraParts_rot[32] =59.763168;
		m_AuroraParts[33] = Vector(218.021814, 0.0, -886.20347);  m_AuroraParts_rot[33] =298.631989;
		m_AuroraParts[34] = Vector(957.705675, 0.0, -776.364808);  m_AuroraParts_rot[34] =255.512802;
		m_AuroraParts[35] = Vector(1684.051544, 0.0, -314.095907);  m_AuroraParts_rot[35] =155.172104;
		m_AuroraParts[36] = Vector(1329.419359, 0.0, -504.566113);  m_AuroraParts_rot[36] =252.343872;
		m_AuroraParts[37] = Vector(1147.565395, 0.0, -638.48327);  m_AuroraParts_rot[37] =214.576508;
		m_AuroraParts[38] = Vector(1561.259444, 0.0, -518.568305);  m_AuroraParts_rot[38] =295.138733;
		m_AuroraParts[39] = Vector(2095.356908, 0.0, -1014.415261);  m_AuroraParts_rot[39] =278.343231;
		m_AuroraParts[40] = Vector(-395.404068, 0.0, -1305.238376);  m_AuroraParts_rot[40] =267.791107;
		m_AuroraParts[41] = Vector(-156.203462, 0.0, -1347.454206);  m_AuroraParts_rot[41] =294.174774;
		m_AuroraParts[42] = Vector(-33.417137, 0.0, -1513.731824);  m_AuroraParts_rot[42] =352.482361;
		m_AuroraParts[43] = Vector(-1040.695502, 0.0, 1467.144627);  m_AuroraParts_rot[43] =295.304871;
		m_AuroraParts[44] = Vector(-1056.476766, 0.0, 1126.882264);  m_AuroraParts_rot[44] =330.060791;
		m_AuroraParts[45] = Vector(-809.448393, 0.0, 1424.321221);  m_AuroraParts_rot[45] =269.391235;
		m_AuroraParts[46] = Vector(-551.086493, 0.0, 1461.2638);  m_AuroraParts_rot[46] =259.729156;
		m_AuroraParts[47] = Vector(-704.554492, 0.0, 1086.954928);  m_AuroraParts_rot[47] =216.733139;
		m_AuroraParts[48] = Vector(-514.316135, 0.0, 1128.517412);  m_AuroraParts_rot[48] =296.992065;
		m_AuroraParts[49] = Vector(-1238.563236, 0.0, 727.726029);  m_AuroraParts_rot[49] =313.150909;
		m_AuroraParts[50] = Vector(-879.747098, 0.0, 993.35101);  m_AuroraParts_rot[50] =274.957092;
		m_AuroraParts[51] = Vector(-335.250003, 0.0, 954.250002);  m_AuroraParts_rot[51] =343.486237;
		m_AuroraParts[52] = Vector(-187.591568, 0.0, 804.067606);  m_AuroraParts_rot[52] =284.395508;
		m_AuroraParts[53] = Vector(-1150.441515, 0.0, 523.342683);  m_AuroraParts_rot[53] =354.059814;
		m_AuroraParts[54] = Vector(-1013.036701, 0.0, 398.426994);  m_AuroraParts_rot[54] =277.985443;
		m_AuroraParts[55] = Vector(-1250.337213, 0.0, -129.627169);  m_AuroraParts_rot[55] =317.571899;
		m_AuroraParts[56] = Vector(-661.431334, 0.0, -189.254322);  m_AuroraParts_rot[56] =350.417542;
		m_AuroraParts[57] = Vector(-536.637633, 0.0, -385.057886);  m_AuroraParts_rot[57] =296.050232;
		m_AuroraParts[58] = Vector(-961.981133, 0.0, -894.71149);  m_AuroraParts_rot[58] =298.631989;
		m_AuroraParts[59] = Vector(-34.582119, 0.0, -825.454939);  m_AuroraParts_rot[59] =270.230682;
		m_AuroraParts[60] = Vector(152.87791, 0.0, 1382.330647);  m_AuroraParts_rot[60] =237.370056;
		m_AuroraParts[61] = Vector(-1768.352465, 0.0, 625.042888);  m_AuroraParts_rot[61] =165.425522;
		m_AuroraParts[62] = Vector(-1638.845913, 0.0, 466.749954);  m_AuroraParts_rot[62] =292.312103;

		int i = 0;
		EntityAI slice;
		vector sliceDir = vector.Zero;

		for (i = 0; i < M_AURORA_CNT_LONG; i++) {
			int auroraVariantLong = Math.RandomInt(1, 7);
			string auroraClassLong = "vfx_aurora_long_" + auroraVariantLong;
			slice = EntityAI.Cast(GetGame().CreateObject(auroraClassLong, m_AuroraParts[i], true, false, false));
			sliceDir[0] = m_AuroraParts_rot[i];
			slice.SetOrientation(sliceDir);
			m_AuroraInstances.Insert(slice);
		}
		for (i = M_AURORA_CNT_LONG; i < (M_AURORA_CNT_LONG + M_AURORA_CNT_SHORT); i++) {
			int auroraVariantShort = Math.RandomInt(1, 7);
			string auroraClassShort = "vfx_aurora_short_" + auroraVariantShort;
			slice = EntityAI.Cast(GetGame().CreateObject(auroraClassShort, m_AuroraParts[i], true, false, false));
			sliceDir[0] = m_AuroraParts_rot[i];
			slice.SetOrientation(sliceDir);
			m_AuroraInstances.Insert(slice);
		}

		vector playerPos;
		if (m_Player) {
			playerPos = m_Player.GetPosition();
		} else {
			playerPos = vector.Zero;
		}

		for (i = 0; i < m_AuroraInstances.Count(); i++) {
			vector newPos;
			newPos[0] = playerPos[0] + m_AuroraParts[i][0];
			newPos[1] = playerPos[1] + m_AuroraAltitude;
			newPos[2] = playerPos[2] + m_AuroraParts[i][2];
			m_AuroraInstances.Get(i).SetPosition(newPos);
		}
	}
	protected void UpdateAuroraSlices()
	{
		vector playerPos = vector.Zero;
		if (m_Player) {
			playerPos = m_Player.GetPosition();
		} else {
			if (GetGame().GetPlayer())
				m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		}

		for (int i = 0; i < m_AuroraInstances.Count(); i++) {
			vector newPos;
			newPos[0] = playerPos[0] + m_AuroraParts[i][0];
			newPos[1] = playerPos[1] + m_AuroraAltitude;
			newPos[2] = playerPos[2] + m_AuroraParts[i][2];
			m_AuroraInstances.Get(i).SetPosition(newPos + m_AuroraOffset);
		}
	}
	protected void RemoveAuroraSlices()
	{
		for (int i = 0; i < m_AuroraInstances.Count(); i++) {
			GetGame().ObjectDelete(m_AuroraInstances.Get(i));
		}
	}
	
	override string GetEventName() 
	{
		return "event-type-aurora";
	}
}