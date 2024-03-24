#include <stdlib.h>
#include <string.h>
#include <inttypes.h> /* PRIu64 */

#include <concord/discord.h>
#include <concord/log.h>

#include "interactions.h"

static void
done_delete_channel(struct discord *cogbot,
                    struct discord_response *resp,
                    const struct discord_channel *channel)
{
    struct cogbot_primitives *primitives = discord_get_data(cogbot);
    const struct discord_interaction *interaction = resp->keep;
    (void)channel;

    /* remove rubberduck role from user */
    discord_remove_guild_member_role(cogbot, primitives->guild_id,
                                     interaction->member->user->id,
                                     primitives->roles.rubberduck_id, NULL, NULL);
}

static void
fail_delete_channel(struct discord *cogbot, struct discord_response *resp)
{
    const struct discord_interaction *interaction = resp->keep;

    discord_edit_original_interaction_response(
        cogbot, interaction->application_id, interaction->token,
        &(struct discord_edit_original_interaction_response){
            .content = "Couldn't delete channel, please contact our staff.",
        },
        NULL);
}

static void
done_get_channel(struct discord *cogbot,
                 struct discord_response *resp,
                 const struct discord_channel *channel)
{
    struct cogbot_primitives *primitives = discord_get_data(cogbot);
    const struct discord_interaction *interaction = resp->keep;

    if (!is_user_rubberduck_channel(channel, primitives->category_id,
                                    interaction->member->user->id))
    {
        discord_edit_original_interaction_response(
            cogbot, interaction->application_id, interaction->token,
            &(struct discord_edit_original_interaction_response){
                .content = "Couldn't complete operation. Make sure to use "
                           "`/mycommand` from your channel",
            },
            NULL);
    }
    else {
        discord_delete_channel(cogbot, channel->id, NULL,
                               &(struct discord_ret_channel){
                                   .done = &done_delete_channel,
                                   .fail = &fail_delete_channel,
                                   .keep = interaction,
                               });
    }
}

void
react_rubberduck_channel_delete(
    struct discord *cogbot,
    struct discord_interaction_response *params,
    const struct discord_interaction *interaction,
    struct discord_application_command_interaction_data_options *options)
{
    bool confirm_action = false;
    char *reason = NULL;

    if (options)
        for (int i = 0; i < options->size; ++i) {
            char *name = options->array[i].name;
            char *value = options->array[i].value;

            if (0 == strcmp(name, "confirm"))
                confirm_action = (0 == strcmp(value, "yes"));
            else if (0 == strcmp(name, "reason"))
                reason = value;
        }

    if (!confirm_action) {
        params->data->content = "No operation will be taking place.";
        return;
    }

    /* TODO: post to a logging channel */
    log_info("User '%s#%s' rubberduck channel deletion (%s)",
             interaction->member->user->username,
             interaction->member->user->discriminator, reason);

    discord_get_channel(cogbot, interaction->channel_id,
                        &(struct discord_ret_channel){
                            .done = &done_get_channel,
                            .fail = &fail_delete_channel,
                            .keep = interaction,
                        });

    params->type = DISCORD_INTERACTION_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE;
}
