modded class Environment
{	
	bool IsSafeFromEVR()
	{
		return (IsUnderRoof() || IsInsideBuilding() || IsInsideVehicle());
	}
}
