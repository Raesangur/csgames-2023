#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/* Type utility */
using instr_args = std::vector<uint32_t*>;
using instr_func = std::function<void(instr_args)>;

/* Function declarations */
static void i_ajout(const instr_args& registers);
static void i_sous(const instr_args& registers);
static void i_multi(const instr_args& registers);
static void i_divi(const instr_args& registers);
static void i_bougelo(const instr_args& registers);
static void i_ou(const instr_args& registers);
static void i_et(const instr_args& registers);
static void i_xor(const instr_args& registers);
static void i_saut(const instr_args& registers);
static void i_sautal(const instr_args& registers);
static void i_sautr(const instr_args& registers);
static void i_beq(const instr_args& registers);
static void i_sauvw(const instr_args& registers);
static void i_loadw(const instr_args& registers);

/* Registers */
static std::array<std::uint32_t, 256> stack = { 0 };
static std::array<std::uint32_t, 4> tx = { 0 };
static std::array<std::uint32_t, 4> sx = { 0 };
static std::uint32_t zero = 0;
static std::uint32_t lo = 0;
static std::uint32_t v0 = 0;
static std::uint32_t ra = -1;
static std::uint32_t sp = 0;
static std::uint32_t pc = 0;
static std::uint32_t imm = 0;


/* Program, instruction and register mapping */
constexpr std::array program1{
  "AJOUTI $s0 $zero 4323",
  "AJOUTI $s1 $zero 438",
  "AJOUT $s1 $s1 $s0",
  "AJOUTI $s2 $zero 2344",
  "AJOUT $t0 $s0 $s2",
  "AJOUTI $s3 $zero 670",
  "XOR $t1 $s0 $s3",
  "AJOUTI $t1 $t1 -4000",
  "ET $t2 $s3 $s1",
  "OU $t3 $t2 $s2",
  "AJOUT $t0 $t0 $t2",
  "SOUS $t0 $t0 $t3",
  "MULTI $t0 $t1",
  "OU $s2 $s2 $t3",
  "BOUGELO $t3",
  "OUI $t3 $t3 6666",
  "ET $s1 $s1 $t3",
  "DIVI $s2 $s3",
  "BOUGELO $t0",
  "AJOUT $s0 $s0 $t0"
};

constexpr std::array program2{
  "AJOUTI $s0 $zero 4323",
  "AJOUTI $s1 $zero 438",
  "AJOUT $s1 $s1 $s0",
  "AJOUTI $s2 $zero 2344",
  "AJOUT $t0 $s0 $s2",
  "AJOUTI $s3 $zero 670",
  "XOR $t1 $s0 $s3",
  "AJOUTI $t1 $t1 -4000",
  "ET $t2 $s3 $s1",
  "OU $t3 $t2 $s2",
  "AJOUTI $t0 $zero 0",
  "AJOUTI $t1 $zero 100",
  "BEQ $t0 $t1 6",
  "AJOUTI $s3 $s3 5",
  "SOUS $t2 $t1 $t0",
  "OU $s3 $s3 $t2",
  "AJOUTI $t0 $t0 1",
  "SAUT 12",
  "SAUTAL 21",
  "AJOUT $s2 $s2 $v0",
  "SAUT 30",
  "SAUVW $s0 0($sp)",
  "SAUVW $s1 4($sp)",
  "AJOUTI $s0 $zero 4323",
  "AJOUTI $s1 $zero 438",
  "AJOUT $s1 $s1 $s0",
  "XOR $v0 $s1 $s0",
  "LOADW $s1 4($sp)",
  "LOADW $s0 0($sp)",
  "SAUTR $ra"
};

const static std::map<const std::string, instr_func> instructionMap
{
  {"AJOUT",   i_ajout},
  {"AJOUTI",  i_ajout},
  {"SOUS",    i_sous},
  {"MULTI",   i_multi},
  {"DIVI",    i_divi},
  {"BOUGELO", i_bougelo},
  {"OU",      i_ou},
  {"OUI",     i_ou},
  {"ET",      i_et},
  {"XOR",     i_xor},
  {"SAUT",    i_saut},
  {"SAUTAL",  i_sautal},
  {"SAUTR",   i_saut},
  {"BEQ",     i_beq},
  {"SAUVW",   i_sauvw},
  {"LOADW",   i_loadw},
};

static std::map<std::string, uint32_t*> registerMap
{
  {"$t0",   &tx[0]},
  {"$t1",   &tx[1]},
  {"$t2",   &tx[2]},
  {"$t3",   &tx[3]},
  {"$s0",   &sx[0]},
  {"$s1",   &sx[1]},
  {"$s2",   &sx[2]},
  {"$s3",   &sx[3]},
  {"$zero", &zero},
  {"$lo",   &lo},
  {"$v0",   &v0},
  {"$ra",   &ra},
  {"$sp",   &sp},
  {"$pc",   &pc},
  {"$imm",  &imm}        // Unexistent register to represent immediates
};

/* Utility functions */
std::vector<std::string> split_string(const std::string& original)
{
	std::stringstream ss{ original };
	std::vector<std::string> v;

	std::string s; // temp string
	// Splitting stream with spaces
	while (std::getline(ss, s, ' ')) {
		v.push_back(s);
	}
	return v;
}

// https://stackoverflow.com/a/4654718
bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c) && c != '-'; }) == s.end();
}

/* Main */
int main()
{
	const auto& program = program2;
	while (pc < program.size())
	{
		// Extract instruction from string
		std::string call = *(program.begin() + pc);
		std::vector<std::string> fullInstruction = split_string(call);
		std::string instruction = fullInstruction[0];

		// Find instruction in tree, check if valid
		auto instructionIt = instructionMap.find(instruction);
		if (instructionIt == instructionMap.end())
		{
			return -1;
		}

		instr_func function = instructionIt->second;

		// Check if the last "register" is an immediate; fill the immediate register and replace the number with the adress of $imm
		std::string lastInstr = fullInstruction.back();
		if (is_number(lastInstr))
		{
			imm = std::stoi(lastInstr);
			fullInstruction.back() = "$imm";
		}

		// Check if the last "register" is an offset-address pair
		auto parenIt = lastInstr.find('(');
		if (parenIt != std::string::npos)
		{
			// Insert immediate in argument list
			imm = std::stoi(lastInstr.substr(0, parenIt));
			fullInstruction.back() = "$imm";

			// Extract register number from between parenthesis
			auto parenIt2 = lastInstr.find(')');
			fullInstruction.push_back(lastInstr.substr(parenIt + 1, parenIt2 - 2));
		}

		// Extract registers from string
		std::vector<std::uint32_t*> registers;
		for (auto it = fullInstruction.begin() + 1; it < fullInstruction.end(); it++)
		{
			auto regIt = registerMap.find(*it);
			if (regIt == registerMap.end())
			{
				return -1;
			}
			registers.push_back(regIt->second);
		}

		// Call function with arguments
		function(registers);
	}

	uint32_t value = sx[0] + sx[1] + sx[2] + sx[3];
	std::string flag = "JDIS-{" + std::to_string(value) + "}";
	std::cout << flag;
	return 0;
}


/* Functions to emulate */

/**
 * AJOUT $d $s $t
 * MIPS: ADD
 * Opération: $d = $s + $t
 * Description: Additionne les valeurs de deux registres et enregistre le résultat dans un autre registre
 */
static void i_ajout(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "AJOUT: Needs 3 arguments" };

	*registers[0] = *registers[1] + *registers[2];
	pc++;
}

/**
 * SOUS $d $s $t
 * MIPS: SUB
 * Opération: $d = $s - $t
 * Description: Soustrait les valeurs de deux registres et enregistre le résultat dans un autre registre
 */
static void i_sous(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "SOUS: Needs 3 arguments" };

	*registers[0] = *registers[1] - *registers[2];
	pc++;
}

/**
 * MULTI $s $t
 * MIPS: MULT
 * Opération: lo = $s * $t
 * Description: Multiplie les valeurs de deux registres et enregistre le résultat dans le registre lo
 */
static void i_multi(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "MULTI: Needs 2 arguments" };

	lo = *registers[0] * *registers[1];
	pc++;
}

/**
 * DIVI $s $t
 * MIPS: DIV
 * Opération: lo = $s / $t
 * Description: Divise les valeurs de deux registres et enregistre le résultat dans le registre lo
 */
static void i_divi(const instr_args& registers)
{
	if (registers.size() != 2)
		throw std::invalid_argument{ "DIVI: Needs 2 arguments" };

	lo = *registers[0] / *registers[1];
	pc++;
}

/**
 * BOUGELO $d
 * MIPS: MFLO
 * Opération: $d=lo
 * Description: Bouge valeur du registre lo dans un autre registre
 */
static void i_bougelo(const instr_args& registers)
{
	if (registers.size() != 1)
		throw std::invalid_argument{ "BOUGELO: Needs 1 argument" };

	*registers[0] = lo;
	pc++;
}

/**
 * OU $d $s $t
 * MIPS: OR
 * Opération: $d = $s | $t
 * Description: Effectue un OU entre les valeurs de deux registres et enregistre le résultat dans un autre registre
 */
static void i_ou(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "OU: Needs 3 arguments" };

	*registers[0] = *registers[1] | *registers[2];
	pc++;
}

/**
 * ET $d $s $t
 * MIPS: AND
 * Opération: $d = $s & $t
 * Description: Effectue un ET entre les valeurs de deux registres et enregistre le résultat dans un autre registre
 */
static void i_et(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "ET: Needs 3 arguments" };

	*registers[0] = *registers[1] & *registers[2];
	pc++;
}

/**
 * XOR $d $s $t
 * MIPS: XOR
 * Opération: $d = $s ^ $t
 * Description: Effectue un OU EXCLUSIF entre les valeurs de deux registres et enregistre le résultat dans un autre registre
 */
static void i_xor(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "XOR: Needs 3 arguments" };

	*registers[0] = *registers[1] ^ *registers[2];
	pc++;
}

/**
 * SAUT target (jump to target)
 * MIPS: J
 * Opération: pc = target
 * Description: Effectue un saut (jump) vers l’addresse de destination
 */
static void i_saut(const instr_args& registers)
{
	if (registers.size() != 1)
		throw std::invalid_argument{ "SAUT: Needs 1 argument" };

	pc = *registers[0];
}

/**
 * SAUTAL target
 * MIPS: JAL
 * Opération: $ra = pc + 1 et pc = target
 * Description: Enregistre l’addresse de retour et effectue un saut (jump) vers l’addresse de destination
 */
static void i_sautal(const instr_args& registers)
{
	if (registers.size() != 1)
		throw std::invalid_argument{ "SAUTAL: Needs 1 argument1" };

	ra = pc + 1;
	pc = *registers[0];
}


/**
 * BEQ $s $t offset
 * MIPS: BEQ
 * Opération: si $s == $t, alors pc = pc + offset
 * Description: Effectue une comparaison suivi d’un saut (jump) si les valeurs des registres sont égales
 */
static void i_beq(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "BEQ: Needs 3 arguments" };

	if (*registers[0] == *registers[1])
		pc += *registers[2];
	else
		pc++;
}

/**
 * SAUVW $t offset($s)
 * MIPS: SW
 * Opération: $($s + offset) = $t
 * Description: Sauvegarde la valeur du registre t à un certain offset de l’addresse contenue dans le registre s
 */
static void i_sauvw(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "SAUVW: Needs 3 arguments" };

	stack[*registers[1] + *registers[2]] = *registers[0];
	pc++;
}

/**
 * LOADW $t offset($s)
 * MIPS: LW
 * Opération: $t = $($s + offset)
 * Description: Load dans le registre t la valeur trouvée à un certain offset de l’addresse contenue dans le registre s
 */
static void i_loadw(const instr_args& registers)
{
	if (registers.size() != 3)
		throw std::invalid_argument{ "LOADW: Needs 3 arguments" };

	*registers[0] = stack[*registers[1] + *registers[2]];
	pc++;
}
