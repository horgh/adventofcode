#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 128
#define MAX_ARG_LENGTH 32
#define REGISTER_COUNT 4

enum InstructionType { CPY, INC, DEC, JNZ, TGL };

struct Instruction {
	enum InstructionType type;
	char * arg0;
	char * arg1;
	int number;
};

static void __destroy_instructions(struct Instruction * const);
static void __destroy_instruction(struct Instruction * const);
static void __print_instruction(const struct Instruction * const);
static int __register_to_index(const char * const);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 3) {
		printf("Usage: %s <input file> <part>\n", argv[0]);
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

	struct Instruction * const instructions =
			calloc(MAX_INSTRUCTIONS, sizeof(struct Instruction));
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

		struct Instruction * const instruction = instructions + instructions_index;
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
		if (strncmp(ptr, "tgl", 3) == 0) {
			instruction->type = TGL;
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
			if (strlen(instruction->arg0) == MAX_ARG_LENGTH - 1) {
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
			if (strlen(instruction->arg1) == MAX_ARG_LENGTH - 1) {
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

	const size_t num_instructions = (size_t)instructions_index;

	if (0) {
		for (size_t i = 0; i < num_instructions; i++) {
			const struct Instruction * const instruction = instructions + i;
			printf("Parsed instruction: ");
			__print_instruction(instruction);
		}
	}

	// Run the program.

	int64_t registers[REGISTER_COUNT];
	memset(registers, 0, sizeof(int64_t) * REGISTER_COUNT);

	// register a is initially 7 in part 1, 12 in part 2.
	if (strcmp(part, "1") == 0) {
		registers[0] = 7;
	} else {
		registers[0] = 12;
	}

	instructions_index = 0;

	// printf("Executing\n");

	while (1) {
		if (instructions_index >= (int)num_instructions) {
			break;
		}

		const struct Instruction * const instruction =
				instructions + instructions_index;

		//__print_instruction(instruction);

		if (instruction->type == CPY) {
			const int dst_register = __register_to_index(instruction->arg1);
			if (dst_register == -1) {
				// Invalid. Must have a target register. It may be invalid because of
				// TGL however.
				instructions_index++;
				continue;
			}

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
			if (dst_register == -1) {
				instructions_index++;
				continue;
			}

			registers[dst_register] += 1;

			instructions_index++;
			continue;
		}

		if (instruction->type == DEC) {
			const int dst_register = __register_to_index(instruction->arg0);
			if (dst_register == -1) {
				instructions_index++;
				continue;
			}

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

			int distance = -1;
			const int r2 = __register_to_index(instruction->arg1);
			if (r2 != -1) {
				distance = (int)registers[r2];
			} else {
				distance = atoi(instruction->arg1);
			}

			// printf("jumping %d\n", distance);
			instructions_index += distance;
			continue;
		}

		if (instruction->type == TGL) {
			const int r = __register_to_index(instruction->arg0);
			int target_instruction = -1;
			if (r != -1) {
				target_instruction = instructions_index + (int)registers[r];
			} else {
				target_instruction = instructions_index + atoi(instruction->arg0);
			}

			// printf("target instruction %d\n", target_instruction);

			// Target might be outside of the program. Do nothing.
			if (target_instruction < 0 ||
					target_instruction >= (int)num_instructions) {
				instructions_index++;
				continue;
			}

			struct Instruction * const instruction2 =
					instructions + target_instruction;

			// printf("toggling ");
			//__print_instruction(instruction2);

			switch (instruction2->type) {
			case CPY:
				instruction2->type = JNZ;
				break;
			case INC:
				instruction2->type = DEC;
				break;
			case DEC:
				instruction2->type = INC;
				break;
			case JNZ:
				instruction2->type = CPY;
				break;
			case TGL:
				instruction2->type = INC;
				break;
			default:
				printf("unknown instruction\n");
				exit(1);
			}

			// printf("it is now ");
			//__print_instruction(instruction2);

			instructions_index++;
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
__destroy_instructions(struct Instruction * const instructions)
{
	if (!instructions) {
		return;
	}

	for (size_t i = 0; i < MAX_INSTRUCTIONS; i++) {
		__destroy_instruction(instructions + i);
	}

	free(instructions);
}

static void
__destroy_instruction(struct Instruction * const instruction)
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
}

static void
__print_instruction(const struct Instruction * const instruction)
{
	switch (instruction->type) {
	case CPY:
		printf("CPY");
		break;
	case INC:
		printf("INC");
		break;
	case DEC:
		printf("DEC");
		break;
	case JNZ:
		printf("JNZ");
		break;
	case TGL:
		printf("TGL");
		break;
	default:
		printf("UNKNOWN");
		break;
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
