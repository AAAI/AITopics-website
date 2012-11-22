This module is an add-on to the FAQ module that allows users with the 
'ask question' permission to create a question which will be queued 
for an 'expert' to answer.

Normal module installation applies.

In order for this module to support multiple experts, it requires that the 
FAQ module be set to "Use Categories."

The settings are on a new tab in the FAQ Settings page.  One selects the roles 
to be considered experts; then one is given a choice of users within those 
roles to associate with the FAQ categories.  The expert role(s) need to be 
given the "answer question" permission.

The module creates a menu item for "Ask a question" or one can create a link to 
ask.  There is an abbreviated version of the FAQ form without an answer field. 
The category choice is not an actual taxonomy selection, but it looks the same. 
The node is created without the 'published' attribute.

There is a block that will show the unanswered questions to the 'expert' 
(generally, this requires a separate role).  The user who created the question 
is also allowed to edit it until it's answered. (After that, standard 
FAQ permissions take over.)

Viewing of the completed question and answer pair is done by the FAQ module.

If notification of question creation is needed, use the Subscriptions module 
set for the appropriate terms (FAQ categories).