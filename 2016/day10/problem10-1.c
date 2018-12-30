// _XOPEN_SOURCE for strdup
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <map.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Instruction {
	char * s;
	bool done;
};

struct Bot {
	int number;
	int chip_low;
	int chip_high;
};

struct Bin {
	int number;
	size_t sz;
	bool * chips;
};

enum ChipType { CHIP_LOW, CHIP_HIGH };

static void __destroy_instructions(struct Instruction **, const size_t);
static bool __all_instructions_done(
		struct Instruction * const * const, const size_t);
static struct Bot * __find_bot(struct htable * const, const int);
static struct Bot * __add_bot(struct htable * const, const int);
static bool __bot_receive_chip(struct Bot * const, int);
static void __destroy_bin(void *);
static void __print_bot(const struct hnode * const, void * const);
static bool __bot_action(
		struct htable * const, struct htable *, const char * const);
static bool __bot_give_chip_to(struct htable * const,
		struct htable *,
		const struct Bot * const,
		const enum ChipType,
		const char * const,
		const int);
static struct Bin * __find_bin(struct htable * const, const int);
static struct Bin * __add_bin(struct htable * const, const int);
static int __bin_get_first_chip(const struct Bin *);

int
main(const int argc, const char * const * const argv)
{
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		return 1;
	}
	const char * const input_file = argv[1];

	FILE * const fh = fopen(input_file, "r");
	if (!fh) {
		printf("Unable to open file: %s: %s\n", input_file, strerror(errno));
		return 1;
	}

	const size_t instructions_sz = 1024;
	struct Instruction ** instructions =
			calloc(instructions_sz, sizeof(struct Instruction *));
	if (!instructions) {
		printf("%s\n", strerror(errno));
		fclose(fh);
		return 1;
	}

	size_t instructions_i = 0;

	while (feof(fh) == 0) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), fh) == NULL) {
			break;
		}

		if (strlen(buf) == 0) {
			continue;
		}

		if (strlen(buf) == 1 && buf[0] == '\n') {
			continue;
		}

		struct Instruction * const instruction =
				calloc(1, sizeof(struct Instruction));
		if (!instruction) {
			printf("%s\n", strerror(errno));
			fclose(fh);
			__destroy_instructions(instructions, instructions_sz);
			return 1;
		}

		instruction->s = strdup(buf);
		;
		if (!instruction->s) {
			printf("%s\n", strerror(ENOMEM));
			fclose(fh);
			__destroy_instructions(instructions, instructions_sz);
			return 1;
		}

		instructions[instructions_i] = instruction;
		instructions_i++;
	}

	if (fclose(fh) != 0) {
		printf("fclose(): %s\n", strerror(errno));
		__destroy_instructions(instructions, instructions_sz);
		return 1;
	}

	struct htable * bots = hash_init(1024);
	if (!bots) {
		printf("hash_init\n");
		__destroy_instructions(instructions, instructions_sz);
		return 1;
	}

	struct htable * output_bins = hash_init(1024);
	if (!output_bins) {
		printf("hash_init\n");
		__destroy_instructions(instructions, instructions_sz);
		hash_free(bots, free);
		return 1;
	}

	while (!__all_instructions_done(instructions, instructions_sz)) {
		for (size_t i = 0; i < instructions_sz; i++) {
			struct Instruction * const instruction = instructions[i];
			if (!instruction) {
				break;
			}

			if (instruction->done) {
				continue;
			}

			if (__bot_action(bots, output_bins, instruction->s)) {
				instruction->done = true;
				// printf("=> Instruction complete: %s", instruction->s);
			}
		}
	}

	__destroy_instructions(instructions, instructions_sz);

	hash_iterate(bots, __print_bot, NULL);
	hash_free(bots, free);

	int value0 = -1;
	const struct Bin * const bin0 = hash_get(output_bins, "0");
	if (!bin0) {
		printf("no bin 0\n");
	} else {
		value0 = __bin_get_first_chip(bin0);
		if (value0 == -1) {
			printf("no chip found in bin 0\n");
		}
	}

	int value1 = -1;
	const struct Bin * const bin1 = hash_get(output_bins, "1");
	if (!bin1) {
		printf("no bin 1\n");
	} else {
		value1 = __bin_get_first_chip(bin1);
		if (value1 == -1) {
			printf("no chip found in bin 1\n");
		}
	}

	int value2 = -1;
	const struct Bin * const bin2 = hash_get(output_bins, "2");
	if (!bin2) {
		printf("no bin 2\n");
	} else {
		value2 = __bin_get_first_chip(bin2);
		if (value2 == -1) {
			printf("no chip found in bin 2\n");
		}
	}

	if (bin0 && bin1 && bin2) {
		printf("%d\n", value0 * value1 * value2);
	}

	hash_free(output_bins, __destroy_bin);

	return 0;
}

static void
__destroy_instructions(struct Instruction ** instructions, const size_t sz)
{
	for (size_t i = 0; i < sz; i++) {
		if (!instructions[i]) {
			break;
		}

		if (instructions[i]->s) {
			free(instructions[i]->s);
		}

		free(instructions[i]);
	}

	free(instructions);
}

static bool
__all_instructions_done(
		struct Instruction * const * const instructions, const size_t sz)
{
	for (size_t i = 0; i < sz; i++) {
		if (!instructions[i]) {
			return true;
		}

		if (!instructions[i]->done) {
			return false;
		}
	}
	return true;
}

static struct Bot *
__find_bot(struct htable * const bots, const int number)
{
	char key[1024];
	memset(key, 0, 1024);
	snprintf(key, 1024, "%d", number);

	struct Bot * bot = hash_get(bots, key);
	return bot;
}

static struct Bot *
__add_bot(struct htable * const bots, const int number)
{
	struct Bot * bot = calloc(1, sizeof(struct Bot));
	if (!bot) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	bot->number = number;
	bot->chip_low = -1;
	bot->chip_high = -1;

	char key[1024];
	memset(key, 0, 1024);
	snprintf(key, 1024, "%d", number);
	if (!hash_set(bots, key, bot)) {
		printf("hash_set\n");
		free(bot);
		return NULL;
	}

	return bot;
}

static bool
__bot_receive_chip(struct Bot * const bot, int chip)
{
	if (bot->chip_low != -1 && bot->chip_high != -1) {
		printf("bot has both chips\n");
		return false;
	}

	// printf("bot %d receives chip %d\n", bot->number, chip);

	if (bot->chip_low == -1) {
		bot->chip_low = chip;
		return true;
	}

	if ((chip == 17 && bot->chip_low == 61) ||
			(bot->chip_low == 17 && chip == 61)) {
		printf("%d\n", bot->number);
	}
	// printf("bot %d comparing chip %d vs %d\n", bot->number, chip,
	// bot->chip_low);

	if (chip < bot->chip_low) {
		bot->chip_high = bot->chip_low;
		bot->chip_low = chip;
		return true;
	}

	bot->chip_high = chip;
	return true;
}

static void
__destroy_bin(void * b)
{
	if (!b) {
		return;
	}

	struct Bin * bin = b;
	if (bin->chips) {
		free(bin->chips);
	}
	free(bin);
}

static void
__print_bot(const struct hnode * const node, void * const unused)
{
	(void)unused;

	const struct Bot * const bot = node->value;

	if (true) {
		return;
	}

	printf(
			"bot %d low: %d high: %d\n", bot->number, bot->chip_low, bot->chip_high);
}

static bool
__bot_action(struct htable * const bots,
		struct htable * output_bins,
		const char * const buf)
{
	// Bot starting out with a chip.

	int chip_number = 0;
	int bot_number = 0;
	int matched =
			sscanf(buf, "value %d goes to bot %d", &chip_number, &bot_number);
	if (matched == 2) {
		struct Bot * bot = __find_bot(bots, bot_number);
		if (!bot) {
			bot = __add_bot(bots, bot_number);
			if (!bot) {
				return false;
			}
		}

		if (!__bot_receive_chip(bot, chip_number)) {
			return false;
		}

		return true;
	}

	// Bot giving a chip.

	char target_type0[1024];
	memset(target_type0, 0, 1024);
	int target0 = -1;

	char target_type1[1024];
	memset(target_type1, 0, 1024);
	int target1 = -1;

	matched = sscanf(buf,
			"bot %d gives low to %s %d and high to %s %d",
			&bot_number,
			target_type0,
			&target0,
			target_type1,
			&target1);
	if (matched != 5) {
		printf("unknown line %s\n", buf);
		return false;
	}

	// Find bot doing the giving.

	struct Bot * const giver_bot = __find_bot(bots, bot_number);
	if (!giver_bot) {
		// printf("giver bot not found (%d)\n", bot_number);
		return false;
	}

	// Careful we don't give away one chip and fail to give away the other. Check
	// we have both before trying to give any away.

	if (giver_bot->chip_low == -1 || giver_bot->chip_high == -1) {
		// printf("bot %d wants to give away chips, but does not have 2\n",
		//		giver_bot->number);
		return false;
	}

	// Give low.

	if (!__bot_give_chip_to(
					bots, output_bins, giver_bot, CHIP_LOW, target_type0, target0)) {
		printf("unable to give low chip: %s", buf);
		return false;
	}

	// Give high.

	if (!__bot_give_chip_to(
					bots, output_bins, giver_bot, CHIP_HIGH, target_type1, target1)) {
		printf("unable to give high chip: %s", buf);
		return false;
	}

	// printf("bot %d gave away chips %d and %d\n", giver_bot->number,
	//		giver_bot->chip_low, giver_bot->chip_high);

	giver_bot->chip_low = -1;
	giver_bot->chip_high = -1;

	return true;
}

static bool
__bot_give_chip_to(struct htable * const bots,
		struct htable * output_bins,
		const struct Bot * const giver_bot,
		const enum ChipType chip_type,
		const char * const target_type,
		const int target_number)
{
	if (chip_type == CHIP_LOW && giver_bot->chip_low == -1) {
		printf("trying to give low chip, but bot %d does not have it\n",
				giver_bot->number);
		return false;
	}

	if (chip_type == CHIP_HIGH && giver_bot->chip_high == -1) {
		printf("trying to give high chip, but bot %d does not have it\n",
				giver_bot->number);
		return false;
	}

	if (strcmp(target_type, "bot") == 0) {
		struct Bot * recip_bot = __find_bot(bots, target_number);
		// May not have bot recorded yet - it may not have received any chips
		// initially.
		if (!recip_bot) {
			recip_bot = __add_bot(bots, target_number);
			if (!recip_bot) {
				return false;
			}
		}

		if (chip_type == CHIP_LOW) {
			if (!__bot_receive_chip(recip_bot, giver_bot->chip_low)) {
				return false;
			}
		} else {
			if (!__bot_receive_chip(recip_bot, giver_bot->chip_high)) {
				return false;
			}
		}

		return true;
	}

	if (strcmp(target_type, "output") == 0) {
		struct Bin * bin = __find_bin(output_bins, target_number);
		if (!bin) {
			bin = __add_bin(output_bins, target_number);
			if (!bin) {
				return false;
			}
		}

		if (chip_type == CHIP_LOW) {
			bin->chips[giver_bot->chip_low] = true;
		} else {
			bin->chips[giver_bot->chip_high] = true;
		}

		return true;
	}

	printf("unknown target type %s\n", target_type);
	return false;
}

static struct Bin *
__find_bin(struct htable * const bins, const int number)
{
	char key[1024];
	memset(key, 0, 1024);
	snprintf(key, 1024, "%d", number);

	struct Bin * bin = hash_get(bins, key);
	return bin;
}

static struct Bin *
__add_bin(struct htable * const bins, const int number)
{
	struct Bin * bin = calloc(1, sizeof(struct Bin));
	if (!bin) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	bin->number = number;

	bin->sz = 1024;

	bin->chips = calloc(bin->sz, sizeof(bool));
	if (!bin->chips) {
		printf("%s\n", strerror(ENOMEM));
		__destroy_bin(bin);
		return NULL;
	}

	char key[1024];
	memset(key, 0, 1024);
	snprintf(key, 1024, "%d", number);
	if (!hash_set(bins, key, bin)) {
		printf("hash_set\n");
		__destroy_bin(bin);
		return NULL;
	}

	return bin;
}

static int
__bin_get_first_chip(const struct Bin * bin)
{
	for (size_t i = 0; i < bin->sz; i++) {
		if (bin->chips[i]) {
			return (int)i;
		}
	}

	return -1;
}
