Below is a comprehensive README-style documentation for the bounties smart contract code. It details the purpose of each action, its parameters, and the overall workflow of how bounties are created and managed. The documentation also explains the key tables used by the contract.

Bounties Contract Documentation
Overview
The bounties contract manages the lifecycle of bounties on the Andromeda platform. A bounty is a reward offered to participants for performing certain tasks. This contract:

Creates and sets up bounty records, including the reward badge and any fungible assets to be distributed.
Manages participant enrollment (open, closed, or external) and enforces submission limits.
Integrates with an external requests system to track submissions and finalize them.
Controls deposit and withdrawal of assets tied to each bounty.
Closes and cleans up bounties once they are settled.
Key Tables
bounty_table
Scope: bounties::get_self().value
Primary Key: emission_symbol.code().raw()
Field	Type	Description
emission_symbol	symbol	Symbol used to identify the bounty, as well as tie-in with the AndEmitter emission system.
badge_symbol	symbol	Symbol of the badge to be used or created for this bounty (if new_or_old_badge is new, it is created later).
status	name	Tracks bounty status such as "init", "setup", "deposited", "closed", etc.
total_fungible_assets	vector<extended_asset>	List of assets (tokens) that must be deposited to fund the bounty.
total_fungible_assets_deposited	vector<extended_asset>	The assets actually deposited so far by the designated payer.
max_fungible_assets_payout_per_winner	vector<extended_asset>	Maximum payout per winner for each asset.
participant_check_contract	name	If open_or_closed_or_external_participation_list == "external", this contract is used for external checks.
participant_check_action	name	The action in participant_check_contract to verify participant eligibility.
participant_check_scope	name	Additional scope info for external checks.
participants	map<name, uint64_t>	List of participants allowed or enrolled in the bounty. The uint64_t often indicates sign-up status.
reviewers	vector<name>	List of accounts authorized to review/approve the participants’ submissions.
participation_start_time	time_point_sec	When participants are allowed to start signing up.
participation_end_time	time_point_sec	When participants are no longer allowed to sign up or submit.
bounty_settlement_time	time_point_sec	When the bounty is settled (submissions must be processed by then).
state_counts	map<name, uint64_t>	Tracks how many submissions are in each state, e.g. approved, processed, etc.
payer	name	The designated account that is obligated to deposit the main bounty assets.
new_or_old_badge	name	Either "new" or "old", indicating if the bounty is using a newly created badge or an existing one.
limited_or_unlimited_participants	name	Either "limited" or "unlimited", indicating whether a max participant limit is enforced.
max_number_of_participants	uint64_t	If "limited", holds the max number of participants.
open_or_closed_or_external_participation_list	name	"open", "closed", or "external", describing how participant eligibility is determined.
num_participants	uint64_t (optional usage)	Tracks the number of participants who have actually signed up.
submissions	map<name, uint64_t>	Tracks how many submissions each participant has made.
max_submissions_per_participant	uint16_t	Limits how many times a participant can submit.
bounty_display_name	string	A human-readable display name for the bounty.
bounty_ipfs_description	string	IPFS hash or link to the bounty description.
emit_contract_assets	vector<contract_asset>	Used to describe which assets (and contract) are emitted via AndEmitter.
bountypool_table
Scope: emission_symbol.code().raw() (the same symbol used in the bounty)
Primary Key: account.value
Tracks additional deposits made by non-payer accounts toward the bounty. These deposits are stored separately from the main deposit.

Field	Type	Description
account	name	The depositing account.
total_fungible_assets_deposited	vector<extended_asset>	Assets deposited by this account.
settings_table
Scope: bounties::get_self().value
Primary Key: setting_name.value
Stores global settings for the bounty contract, such as:

"minothers": A basis-point value specifying the minimum deposit required by non-payer accounts, relative to the total needed for the bounty.
actionauths_table
Scope: org.value
Primary Key: action.value
Tracks which accounts are authorized to invoke specific actions on behalf of a given organization (org).

Field	Type	Description
action	name	The action name.
authorized_accounts	vector<name>	List of accounts allowed to call the action.
Actions
Below is a description of each action in the bounties contract.

1. newbounty
Definition:

cpp
Copy
Edit
ACTION bounties::newbounty(
   name authorized,
   symbol emission_symbol,
   name payer,
   string bounty_display_name,
   string bounty_ipfs_description,
   vector<asset> emit_assets,
   vector<extended_asset> total_fungible_assets,
   vector<extended_asset> max_fungible_assets_payout_per_winner,
   uint16_t max_submissions_per_participant,
   vector<name> reviewers,
   time_point_sec participation_start_time,
   time_point_sec participation_end_time,
   time_point_sec bounty_settlement_time,
   name new_or_old_badge,
   name limited_or_unlimited_participants,
   name open_or_closed_or_external_participation_list
);
Purpose:

Initializes a bounty record in the bounty_table.
Establishes metadata (e.g., display name, IPFS description).
Sets parameters such as participation windows, submission limits, and the reviewer list.
Marks whether a new badge must be created or an old badge is reused.
Declares whether the participant list is open, closed, or driven by an external contract.
Optionally sets the status to "setup" if further actions are needed (like badge creation or participant list setup).
Key Steps:

Authorization: Validates that authorized can invoke newbounty on behalf of the organization derived from emission_symbol.
Time Validations: Ensures the bounty’s participation and settlement periods are in the future and logically consistent.
Parameter Checks: Confirms that enumeration fields (new_or_old_badge, limited_or_unlimited_participants, open_or_closed_or_external_participation_list) are valid.
Insert Bounty: Emplaces a new record in the bounty_table.
Sets Initial Status: Usually to "setup" if additional configuration is needed (badge or participant listing). Otherwise, defaults to "init".
2. oldbadge
cpp
Copy
Edit
ACTION bounties::oldbadge(
   name authorized,
   symbol emission_symbol,
   symbol old_badge_symbol
);
Purpose:

Assigns an existing badge (old_badge_symbol) to a bounty that expects an old badge.
Updates the bounty record and potentially completes its setup if all requirements are met.
Key Steps:

Authorization: Checks that authorized can call oldbadge.
Finds Bounty: Looks up the bounty by emission_symbol.
Validates: Ensures the bounty expects an old badge (new_or_old_badge == "old") and the badge symbol has not yet been set.
Update: Sets the badge_symbol in the bounty_table entry.
Check Setup Completion: Calls check_and_update_status to see if all setup requirements are now met. If yes, it transitions the bounty to "init" and triggers AndEmitter emission creation.
Optional Feature Registration: Calls BADGEDATA_CONTRACT::addfeature to register the badge with the AndEmitter contract or other consumers.
3. newbadge
cpp
Copy
Edit
ACTION bounties::newbadge(
   name authorized,
   symbol emission_symbol,
   symbol new_badge_symbol,
   string badge_display_name,
   string badge_ipfs_image,
   string badge_description,
   string badge_creation_memo,
   bool lifetime_aggregate,
   bool lifetime_stats
);
Purpose:

Creates a new badge for a bounty that expects a new badge.
Updates the bounty record and triggers creation of the badge on SIMPLEBADGE_CONTRACT.
Optionally enables lifetime aggregates and/or statistics on CUMULATIVE_CONTRACT or STATISTICS_CONTRACT.
Key Steps:

Authorization: Checks if authorized can invoke newbadge.
Validates: Ensures the bounty record is expecting a new badge (new_or_old_badge == "new") and the new badge is not yet set.
Badge Creation: Calls SIMPLEBADGE_CONTRACT::create to define a brand new badge.
Update: Saves new_badge_symbol to the bounty record.
Check Setup Completion: Calls check_and_update_status.
Optional Feature Registration: Registers additional features such as lifetime aggregates or stats.
4. limited
cpp
Copy
Edit
ACTION bounties::limited(name authorized, symbol emission_symbol, uint64_t max_number_of_participants);
Purpose:

Used to set max_number_of_participants for a bounty with limited_or_unlimited_participants == "limited".
Key Steps:

Authorization: Validates caller.
Finds Bounty: Ensures the bounty exists and is set to "limited" participation.
Updates: Stores max_number_of_participants in the bounty record and checks if setup is complete.
5. closed
cpp
Copy
Edit
ACTION bounties::closed(
   name authorized,
   symbol emission_symbol,
   vector<name> participants
);
Purpose:

Populates a closed participation list with the specified participants.
Marks them in the bounty record as (participant -> 0) by default.
Key Steps:

Authorization: Validates caller.
Closed Check: Ensures the bounty is set to "closed" for open_or_closed_or_external_participation_list.
List: Initializes all given participants to a default sign-up value of 0.
Completion: Invokes check_and_update_status.
6. external
cpp
Copy
Edit
ACTION bounties::external(
   name authorized,
   symbol emission_symbol,
   name participant_check_contract,
   name participant_check_action,
   name participant_check_scope
);
Purpose:

Configures an external contract/action that is used to validate participant eligibility.
Relevant for open_or_closed_or_external_participation_list == "external".
Key Steps:

Authorization: Validates caller.
External Check: Ensures the bounty is set to "external" participation.
Updates: Saves the contract, action, and optional scope into the bounty.
Completion: Calls check_and_update_status.
7. participants
cpp
Copy
Edit
ACTION bounties::participants(
   name authorized, 
   symbol emission_symbol, 
   vector<name> participants
);
Purpose:

Adds participants to an existing closed bounty (appending to the already stored participant list).
Key Steps:

Authorization: Checks the caller.
Closed Check: Bounty must be open_or_closed_or_external_participation_list == "closed".
Updates: Inserts new participants into the participants map if they are not already present.
8. reviewers
cpp
Copy
Edit
ACTION bounties::reviewers(
   name authorized,
   symbol emission_symbol,
   vector<name> reviewers
);
Purpose:

Appends reviewers to the existing reviewers list for the bounty.
Key Steps:

Authorization: Checks the caller.
Update: Adds new reviewer accounts to the reviewers field, avoiding duplicates.
9. ontransfer (Handler)
cpp
Copy
Edit
void bounties::ontransfer(name from, name to, asset amount, string memo);
Purpose:

Receiver hook for standard token transfers.
Handles both payer deposits to fund the bounty and non-payer contributions.
Ensures that deposits align with what the bounty expects and updates the bounty status as it becomes fully funded.
Key Steps:

Check: Only process if to == get_self() and from != get_self().
Bounty Lookup: Parse memo as a symbol code.
Match Asset: Verify that the transferred amount matches one of the total_fungible_assets.
Payer Branch: If from == bounty.payer, deposit into the bounty, potentially moving status to "deposited" if fully funded. Otherwise, refund if exceeding the needed amount.
Non-Payer Branch: Enforce a minimum deposit (basis points from settings_table), and store the deposit in bountypool_table.
10. status (Callback)
cpp
Copy
Edit
void bounties::status(
   name requester,
   uint64_t request_id,
   name originating_contract,
   name originating_contract_key,
   name old_status,
   name new_status
);
Purpose:

Callback typically invoked by a request/approval system to notify the bounty about submission status changes.
Adjusts the state_counts in the bounty record.
Triggers distribution if something moves to "processed" or handles partial withdrawal if "withdrawn".
Key Steps:

Identify Bounty: Convert originating_contract_key to uppercase, interpret as a symbol, and find the corresponding record.
State Counts: Decrement the old_status count (unless it’s "approved" -> "processed") and increment the new_status.
Distribution: If new_status == "processed", calls distribute(...).
Withdrawn: If new_status == "withdrawn", calls handle_withdrawal(...).
11. signup
cpp
Copy
Edit
ACTION bounties::signup(name participant, symbol emission_symbol, string stream_reason);
Purpose:

Records a participant’s intention to join (sign up) for the bounty.
Checks the bounty’s status and sign-up rules.
Key Steps:

Require Auth: Must be signed by participant.
Time Checks: Must be within [participation_start_time, participation_end_time).
Status Check: Bounty must be "deposited".
Badge Check: badge_symbol must be set.
Closed: Requires the participant to already be in the participants map.
External: Invokes the external check contract if configured.
Open: Anyone can sign up, but enforces participant limits if limited.
Mark Signup: Sets participants[participant] to 1 and increments num_participants.
12. cancelsignup
cpp
Copy
Edit
ACTION bounties::cancelsignup(name participant, symbol emission_symbol, string stream_reason);
Purpose:

Allows a participant to cancel their sign-up if they have no submissions yet.
Key Steps:

Require Auth: Must be signed by participant.
Find Bounty: Ensure it exists.
Check: The participant must already be enrolled, and must have zero submissions.
Closed: Sets participants[participant] = 0.
Open/External: Erases the participant from the participants map entirely.
13. submit
cpp
Copy
Edit
ACTION bounties::submit(name participant, symbol emission_symbol, string stream_reason);
Purpose:

Initiates a new submission from a participant.
Calls an external ingestsimple action on the requests contract to record/approve the submission.
Key Steps:

Require Auth: Must be signed by participant.
Time & Status Check: Must be within participation time, and bounty must be "deposited".
Sign-Up Check: The participant must already have signed up (entry = 1).
Submission Count: Increment the user’s count in submissions, ensuring it doesn’t exceed max_submissions_per_participant.
Requests Contract: Calls requests::ingestsimple with relevant info (badge details, reviewers, etc.).
14. withdraw
cpp
Copy
Edit
ACTION bounties::withdraw(name account, symbol emission_symbol);
Purpose:

Allows either the designated payer or a non-payer to withdraw their funds from the bounty once certain conditions are met.
Key Steps:

Require Auth: Must be signed by account.
Find Bounty: Looks up by emission_symbol.
Payer Withdrawal: Only allowed if the bounty status == "closed". Transfers entire balance back to the payer, clearing total_fungible_assets_deposited.
Non-Payer Withdrawal: Looks up the deposit in bountypool_table. Transfers the non-payer’s assets back to them, then erases their record in bountypool_table.
15. closebounty
cpp
Copy
Edit
ACTION bounties::closebounty(symbol emission_symbol);
Purpose:

Marks a bounty as "closed" if all "approved" submissions have been "processed".
Key Steps:

Lookup: Finds bounty by emission_symbol.
Check: Ensures approved == processed in state_counts.
Update: Sets bounty.status = "closed".
16. cleanup
cpp
Copy
Edit
ACTION bounties::cleanup(symbol emission_symbol);
Purpose:

Deletes the bounty record entirely if status == "closed" and all deposits are withdrawn.
Key Steps:

Lookup: Finds the bounty in bounty_table.
Checks: Must be "closed" and have zero total_fungible_assets_deposited.
Deactivate Emission: Calls deactivate_emission for the associated emission.
Erase: Removes the bounty record.
17. addactionauth
cpp
Copy
Edit
ACTION bounties::addactionauth(name org, name action, name authorized_account);
Purpose:

Grants authorized_account permission to call a particular action on behalf of org.
Key Steps:

Auth: Must be signed by org.
Upsert: Finds or creates an entry in actionauths_table for (org, action). Adds authorized_account to the list if not already present.
18. delactionauth
cpp
Copy
Edit
ACTION bounties::delactionauth(name org, name action, name authorized_account);
Purpose:

Removes authorized_account from the authorized list for (org, action).
Key Steps:

Auth: Must be signed by org.
Modify: Removes authorized_account from the vector in actionauths_table.
Cleanup: If no accounts remain, the entire entry is erased.
Internal Helper Methods
check_and_update_status(bounty &row)
Purpose:

Checks if all necessary setup fields in a bounty are complete.
If everything is ready, transitions the bounty’s status to "init" and triggers an emission creation/activation on ANDEMITTER_CONTRACT.
Checks:

badge_symbol is set.
If limited, max_number_of_participants is set.
If closed, participants list is not empty.
If external, participant_check_contract and participant_check_action are set.
handle_withdrawal(symbol target_sym, name requester)
Purpose:

Decrements the submission count for a participant who withdraws a submission (new_status == "withdrawn").
Contract Workflow Summary
Create a Bounty:

Call newbounty to create a new bounty record.
Depending on new_or_old_badge, you must call either newbadge or oldbadge.
If limited, call limited to set max_number_of_participants.
If open_or_closed_or_external_participation_list is closed, call closed (or later participants) to set the allowed participant list. If external, call external to set the external checker.
Deposit Funding:

The designated payer sends the required tokens via a standard transfer with the memo containing the bounty’s symbol code. Once fully funded, the bounty moves to deposited status.
Third parties can also deposit tokens into the bounty’s pool, subject to a minimum deposit requirement.
Participants Sign Up:

They must call signup (for open, closed, or external bounties). If closed, the participant must already be in the allowed list.
If external, the sign-up triggers the external check contract.
Submissions:

Participants call submit to create a submission. The contract then calls an external system (requests::ingestsimple) to track and review the submission.
Each submission updates the internal count for that participant.
Status Updates & Distribution:

The external requests/approval system calls status on this contract whenever a submission’s state changes (e.g., to "approved", "processed", "withdrawn", etc.).
When a submission transitions to "processed", distribute is triggered to distribute any bounty rewards.
When a submission transitions to "withdrawn", the participant’s submission count is decremented.
Close & Cleanup:

Once all "approved" submissions are "processed", call closebounty. This sets the bounty’s status to "closed".
The payer (and non-payer contributors) can withdraw any remaining funds by calling withdraw.
Finally, if the bounty is "closed" and fully emptied, cleanup can be called to remove the bounty from the table and deactivate its emission.
Conclusion
The bounties contract provides a flexible system for creating, funding, and managing bounties on the Andromeda platform. It supports different methods of participant enrollment (open, closed, external), multiple depositors, integration with a badge system, and calls out to external request/approval contracts for processing submissions. Once a bounty is finished (all approved submissions processed), it can be closed and eventually cleaned up, returning funds to the depositor(s).

If you have any further questions about usage, see the action-by-action references above or consult higher-level application or UI logic that interacts with these actions.
