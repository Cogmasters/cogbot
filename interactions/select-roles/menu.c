#include <string.h>
#include <inttypes.h> /* PRIu64 */

#include <concord/discord.h>

#include "interactions.h"

void
react_select_subscriptions_menu(struct discord *cogbot,
                                struct discord_interaction_response *params,
                                const struct discord_interaction *interaction)
{
    struct cogbot_primitives *primitives = discord_get_data(cogbot);
    struct discord_guild_member *member = interaction->member;

    u64_snowflake_t roles[3] = { 0 };
    bool is_reset = false;

    /* get channel visibility */
    if (interaction->data->values)
        for (int i = 0, j = 0; interaction->data->values[i]; ++i) {
            char *value = interaction->data->values[i]->value;

            if (0 == strcmp(value, "announcements"))
                roles[j++] = primitives->roles.announcements_id;
            else if (0 == strcmp(value, "watcher"))
                roles[j++] = primitives->roles.watcher_id;
            else if (0 == strcmp(value, "reset"))
                is_reset = true;
        }

    if (is_reset) {
        discord_remove_guild_member_role(
            cogbot, interaction->guild_id, member->user->id,
            primitives->roles.announcements_id, NULL);

        discord_remove_guild_member_role(cogbot, interaction->guild_id,
                                         member->user->id,
                                         primitives->roles.watcher_id, NULL);

        params->data->content = "Your subscriptions have been reset";
    }
    else {
        const int arr_size = sizeof(roles) / sizeof(u64_snowflake_t);

        for (int i = 0; roles[i] && i < arr_size; ++i) {
            discord_add_guild_member_role(cogbot, interaction->guild_id,
                                          member->user->id, roles[i], NULL);
        }

        params->data->content = "Your subscriptions have been registered";
    }
}

void
react_select_OS(struct discord *cogbot,
                struct discord_interaction_response *params,
                const struct discord_interaction *interaction)
{
    struct cogbot_primitives *primitives = discord_get_data(cogbot);
    struct discord_guild_member *member = interaction->member;

    u64_snowflake_t roles[2] = { 0 };
    bool is_reset = false;

    /* get channel visibility */
    if (interaction->data->values)
        for (int i = 0, j = 0; interaction->data->values[i]; ++i) {
            char *value = interaction->data->values[i]->value;

            if (0 == strcmp(value, "linux"))
                roles[j++] = primitives->roles.linux_id;
            if (0 == strcmp(value, "windows"))
                roles[j++] = primitives->roles.windows_id;
            if (0 == strcmp(value, "macos"))
                roles[j++] = primitives->roles.macos_id;
            else if (0 == strcmp(value, "reset"))
                is_reset = true;
        }

    if (is_reset) {
        discord_remove_guild_member_role(cogbot, interaction->guild_id,
                                         member->user->id,
                                         primitives->roles.linux_id, NULL);
        discord_remove_guild_member_role(cogbot, interaction->guild_id,
                                         member->user->id,
                                         primitives->roles.windows_id, NULL);
        discord_remove_guild_member_role(cogbot, interaction->guild_id,
                                         member->user->id,
                                         primitives->roles.macos_id, NULL);

        params->data->content = "Your OS has been reset";
    }
    else {
        const int arr_size = sizeof(roles) / sizeof(u64_snowflake_t);

        for (int i = 0; roles[i] && i < arr_size; ++i) {
            discord_add_guild_member_role(cogbot, interaction->guild_id,
                                          member->user->id, roles[i], NULL);
        }

        params->data->content = "Your OS has been set";
    }
}
