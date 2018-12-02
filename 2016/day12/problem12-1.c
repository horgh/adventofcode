#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 128
#define MAX_ARG_LENGTH 32
#define REGISTER_COUNT 4

enum InstructionType {CPY, INC, DEC, JNZ};

struct Instruction {
	enum InstructionType type;
	char * arg0;
	char * arg1;
	int number;
};

static void
__destroy_instructions(struct Instruction * *);
static void
__destroy_instruction(struct Instruction *);
static void
__print_instruction(const struct Instruction * const);
static int
__register_to_index(const char * const);

int main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input file> <part #>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];
	char const * const part = argv[2];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	// Read in the program's instructions.

	struct Instruction * * instructions = calloc(MAX_INSTRUCTIONS,
			sizeof(struct Instruction *));
	if (!instructions) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	int instructions_index = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		struct Instruction * instruction = calloc(1, sizeof(struct Instruction));
		if (!instruction) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_instructions(instructions);
			return 1;
		}

		instructions[instructions_index] = instruction;
		instructions_index++;

		const char * ptr = buf;

		if (strlen(ptr) < 4) {
			printf("instruction too short\n");
			fclose(fh);
			__destroy_instructions(instructions);
			return 1;
		}

		if (strncmp(ptr, "cpy", 3) == 0) {
			instruction->type = CPY;
		}
		if (strncmp(ptr, "inc", 3) == 0) {
			instruction->type = INC;
		}
		if (strncmp(ptr, "dec", 3) == 0) {
			instruction->type = DEC;
		}
		if (strncmp(ptr, "jnz", 3) == 0) {
			instruction->type = JNZ;
		}

		ptr += 3;

		if (*ptr != ' ') {
			printf("unexpected end of instruction\n");
			fclose(fh);
			__destroy_instructions(instructions);
			return 1;
		}

		ptr++;

		// Argument 1.

		instruction->arg0 = calloc(MAX_ARG_LENGTH, sizeof(char));
		if (!instruction->arg0) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_instructions(instructions);
			return 1;
		}

		while (*ptr != ' ' && *ptr != '\n' && *ptr != 0) {
			if (strlen(instruction->arg0) == MAX_ARG_LENGTH-1) {
				printf("argument 0 is too long\n");
				fclose(fh);
				__destroy_instructions(instructions);
				return 1;
			}
			strncat(instruction->arg0, ptr, 1);
			ptr++;
		}

		if (*ptr == '\n' || *ptr == 0) {
			continue;
		}

		ptr++;

		// Argument 2.

		instruction->arg1 = calloc(MAX_ARG_LENGTH, sizeof(char));
		if (!instruction->arg1) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_instructions(instructions);
			return 1;
		}

		while (*ptr != '\n' && *ptr != 0) {
			if (strlen(instruction->arg1) == MAX_ARG_LENGTH-1) {
				printf("argument 1 is too long\n");
				fclose(fh);
				__destroy_instructions(instructions);
				return 1;
			}
			strncat(instruction->arg1, ptr, 1);
			ptr++;
		}
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_instructions(instructions);
		return 1;
	}

	for (size_t i = 0; i < MAX_INSTRUCTIONS; i++) {
		const struct Instruction * const instruction = instructions[i];
		if (!instruction) {
			continue;
		}
		if (0) {
			printf("Parsed instruction: ");
			__print_instruction(instruction);
		}
	}

	// Run the program.

	int64_t registers[REGISTER_COUNT];
	memset(registers, 0, sizeof(int64_t)*REGISTER_COUNT);

	// For part 2, initialize register c to 1.
	// For part 1, leave this 0.
	if (strcmp(part, "2") == 0) {
		registers[2] = 1;
	}

	instructions_index = 0;

	while (1) {
		const struct Instruction * const instruction =
			instructions[instructions_index];
		if (!instruction) {
			//printf("reached invalid instruction\n");
			break;
		}

		//printf("Executing ");
		//__print_instruction(instruction);

		if (instruction->type == CPY) {
			const int dst_register = __register_to_index(instruction->arg1);
			const int src_register = __register_to_index(instruction->arg0);
			if (src_register != -1) {
				registers[dst_register] = registers[src_register];
			} else {
				registers[dst_register] = atoi(instruction->arg0);
			}
			instructions_index++;
			continue;
		}

		if (instruction->type == INC) {
			const int dst_register = __register_to_index(instruction->arg0);
			registers[dst_register] += 1;
			instructions_index++;
			continue;
		}

		if (instruction->type == DEC) {
			const int dst_register = __register_to_index(instruction->arg0);
			registers[dst_register] -= 1;
			instructions_index++;
			continue;
		}

		if (instruction->type == JNZ) {
			const int r = __register_to_index(instruction->arg0);
			if (r != -1) {
				if (registers[r] == 0) {
					instructions_index++;
					continue;
				}
			} else {
				if (atoi(instruction->arg0) == 0) {
					instructions_index++;
					continue;
				}
			}

			const int distance = atoi(instruction->arg1);
			//printf("jumping %d\n", distance);
			instructions_index += distance;
			continue;
		}

		printf("invalid instruction\n");
		break;
	}

	printf("%" PRId64 "\n", registers[0]);

	__destroy_instructions(instructions);

	return 0;
}

static void
__destroy_instructions(struct Instruction * * instructions)
{
	if (!instructions) {
		return;
	}

	for (size_t i = 0; i < MAX_INSTRUCTIONS; i++) {
		struct Instruction * instruction = instructions[i];
		__destroy_instruction(instruction);
	}

	free(instructions);
}

static void
__destroy_instruction(struct Instruction * instruction)
{
	if (!instruction) {
		return;
	}

	if (instruction->arg0) {
		free(instruction->arg0);
	}

	if (instruction->arg1) {
		free(instruction->arg1);
	}

	free(instruction);
}

static void
__print_instruction(const struct Instruction * const instruction)
{
	switch (instruction->type) {
		case CPY: printf("CPY"); break;
		case INC: printf("INC"); break;
		case DEC: printf("DEC"); break;
		case JNZ: printf("JNZ"); break;
		default: printf("UNKNOWN"); break;
	}

	printf(" %s", instruction->arg0);

	if (instruction->arg1) {
		printf(" %s", instruction->arg1);
	}

	printf("\n");
}

static int
__register_to_index(const char * const s)
{
	if (strcmp(s, "a") == 0) {
		return 0;
	}
	if (strcmp(s, "b") == 0) {
		return 1;
	}
	if (strcmp(s, "c") == 0) {
		return 2;
	}
	if (strcmp(s, "d") == 0) {
		return 3;
	}
	return -1;
}
