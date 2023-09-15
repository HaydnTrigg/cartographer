#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/game/game_options.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/tag_files/string_id.h"

namespace PlayerRepresentation
{
	/**
	 * \brief Adds a new player representation to the globals tag block
	 * \param fp_hands the datum index of the first person hands render model
	 * \param fp_body the datum index of the first person body render model
	 * \param tp_biped the datum index of the third person biped
	 * \param type the type of biped being added, will override an existing biped
	 * \param variant the string_id of the variant
	 * \return returns a pointer to the new representation
	 */
	s_game_globals_player_representation* add_representation(datum fp_hands, datum fp_body, datum tp_biped, e_character_type type, string_id variant = -1);
	/**
	 * \brief Clones an existing player_representation
	 * \param index 0 based index of the representation to clone
	 * \param newType the new Player::Biped type to register this as, cannot be the same as an existing one.
	 * \return returns a pointer to the new representation
	 */
	s_game_globals_player_representation* clone_representation(int index, e_character_type newType);

	datum get_object_datum_from_representation(e_character_type representation_index);
	void OnMapLoad(s_game_options* options);
	void Initialize();
}
