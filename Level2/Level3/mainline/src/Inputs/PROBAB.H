#pragma once

class ProbabilityDistribution;
class ArctermFile;
class InputTerminal;
class CProbDistManager;
// Reads a probability distribution into the appropriate subclass of
// ProbabilityDistribution and returns a generic pointer to it
// Assumes that the current position is valid
// Exception: FileFormatError
ProbabilityDistribution* GetTerminalRelateProbDistribution(ArctermFile &p_file, InputTerminal * pInterm, bool _bHandleProcDistWithPipe = false);

ProbabilityDistribution* GetProbDistribution(ArctermFile &p_file);








CProbDistManager* _g_GetActiveProbMan( InputTerminal* _pInterm );
