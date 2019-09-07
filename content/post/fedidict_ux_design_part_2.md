---
title: "FediDict UX Design, Part 2"
slug: "fedidict_ux_design_part_2"
date: 2018-09-02 00:00:00+00:00
categories:
- Fedidict
- Programming
- Rust
description: More in-depth analysis and design for the UX of FediDict, focused on security.
---

*This is the second of several posts on the design of my current ActivityPub-enabled
Rust web application, FediDict. Each of these posts was be released to my Patreon
patrons before being made publicly available. I recommend that you read the [first
UX design post](/post/fedidict_ux_design_part_1) first.*

Any software that accepts input over the network will, eventually, be subject to
attack, of some kind. Any federated software service must accept input over the network -
that's the whole point. In the case of FediDict, these attacks could come from either the
user-facing side or the federation-facing, or server-to-server, side.

Either way, some of the technology choices I've made will help FediDict withstand attack,
and in this post, I'll explore how a solid access control model can help secure a system
while still enabling rich control by administrators and a good user experience.

# Inherent Sturdiness

FediDict is built in Rust, a language that promises memory safety for all programs written
without the `unsafe` keyword. This eliminates (or, at least, mitigates to the point of
insignificance) the majority of common memory-based attacks, like buffer overruns,
use-after-free bugs, double-free bugs, and other such issues.

I'm also using a proven database system, though I'm not sure if PostgreSQL or MariaDB will
be my final choice yet. Either one has a track record of high security, and FediDict will
interface with them using Diesel, an object-relational mapping library which uses static
analysis to ensure that certain types of database errors and bugs (like SQL injection) are
not possible.

# Access Control

Nonetheless, bugs exist in any piece of software, and application-level bugs are often far
more impactful than are those in underlying code, precisely because they are so hard to
predict and expose precisely what the attacker wants - the data held in the application.

One of the best ways to mitigate application-level bugs is to have a consistent (and
consistently enforced) access control model. Rust's type system helps me here.

## A Pyramid Model

A dictionary produced by a small team of professionals is an easy thing to secure, but a
crowdsourced (or, a semi-crowdsourced) one is not. In FediDict, I'll have a few basic
levels of access:

1. The Guest. A guest has absolutely no write access, at all, but they have read access
    to most things. I'll define exactly what later.
2. The Contributor. A contributor has all the privelages a Guest has, as well as write
    access to the moderation queue (but, you'll note, not to the site directly), and to
    the vote count of existing definitions, and to the report queue.
3. The Moderator. A moderator has all the privelages a Contributor has, as well as being
    able to read the moderation queue and write from the moderation queue to the public
    database of definitions.

This is a tidy and convenient pyramid of access. Each level builds on the last, each group
is smaller than the last, and all these privelages can be enforced with simple conditions,
even at the database level.

Access becomes slightly more complex when I bring federated users into the mix. Because
ActivityPub supports "liking" posts remotely, and I wish to support this as well, some
people can write to the vote count of definitions without being Contributors.


## Roles, not Tiers

This would appear to pose a problem, but in fact, it's easy to solve with a concept known
as Role Based Access Control, or RBAC. RBAC is a technique used by many complex systems,
including [Amazon Web Services](https://aws.amazon.com/iam/) and [Google Cloud](https://cloud.google.com/iam/),
but is applicable to simpler systems as well.

With RBAC, I'll define a set of roles and assign those roles to users based on various
factors. Then, when a user attempts to perform an action, FediDict will check for these
roles. For instance, the roles could be:

- **DefinitionReader**: can access the public database of definitions.
- **DefinitionSubmitter**: can submit new definitions to the moderation queue.
- **DefinitionEvaluator**: can submit likes and reports for definitions.
- **DefinitionRemover**: can remove definitions from the public database.
- **QueueReader**: can read definitions submitted to the moderation queue.
- **QueueApprover**: can move definitions from the moderation queue to the public database.
- **QueueRejecter**: can remove definitions from the moderation queue.
- **AccountCreator**: can create new local FediDict accounts.
- **AccountRemover**: can remove local FediDict accounts.
- **AccountRoleAssigner**: can assign and remove roles on accounts.

I could use these roles in Rust through an `enum`. I'm not sure just how it will look,
but for example:

```rust
impl Definition {
    // ... other code for definitions
    /// approve() sets a definition as approved, by the authority of the
    /// given user.
    fn approve(self, authority: &User) -> Result<Definition, Error> {
        if authority.has_role(Roles::QueueApprover) {
            let mut new = self;
            new.approved_by = Some(authority.id);
            return Ok(new);
        } else {
            return Err(Error::PermissionDenied(
                        Roles::QueueApprover,
                        "approve a definition from the moderation queue");
        }
    }
```

This could then be used in a pipeline. For instance: 

```rust
// Assume I have user, the current user, and def, a definition, from elsewhere
// in the code.
def.approve(&user)?.commit()?;
```

In the case of an error, the information given is enough to render a message like:

> Error: Permission denied. In order to approve a definition from the moderation queue,
> you need the QueueApprover role, which you do not have. Contact an administrator for
> assistance.

## More Flexibility Without (Much) Complexity

I can still model tiers with these roles. A Guest has only the **DefinitionReader**
role, while a Contributor has **DefinitionReader**, **DefinitionSubmitter**, and
**DefinitionEvaluator**. A Moderator has all of those roles, in addition to
**QueueReader**, **QueueApprover**, and **QueueRejecter**.

I can also construct other types of user from these roles. For instance, a site admin
needs **AccountCreator**, **AccountRemover**, and **AccountRoleAssigner**, but
could delegate account creation to others by giving them only **AccountCreator**.

Someone the organization trusts to remove spam but not to fully evaluate the validity of
new definitions could have **QueueReader** and **QueueRejecter** but _not_
**QueueApprover**.

A user interacting over ActivityPub can like and report as well as viewing the database,
so they would get **DefinitionReader** and **DefinitionEvaluator**, but not any other
roles.

With roles, I get the security provided by a solid tiered model with a flexibility that
a tiered model can never provide.

## Implementing Roles

This might seem like a difficult technique to implement, but actually, it's very natural,
especially using the combination of Rust's expressive type system and a powerful RBDMS
like PostgreSQL.

Each user will have a database record in the Users table with an associated ID.
That record will specify their user name, e-mail address, (hashed) password, and any
other information, including the ID of a row in the Roles table.

The Roles table will associate user IDs with a set of roles. Each role can be on, off,
or none. This maps naturally to a nullable Boolean value in SQL, or to an `Option<bool>`
in Rust.

Say, for instance, ther are three users, an admin, a moderator, and a contributor.
The users table might hold three records, like so:

ID | Username | email | password | roles |
---|----------|-------|----------|-------|
12 | ltindall | a@b.c | d7239dhs | 02358 |
87 | chughes  | c@x.y | a2342nb0 | 83950 |
34 | djanes   | q@m.z | passw0rd | 22741 |

And, the Roles table would have some associated records:

ID   | definition-reader | definition-submitter | queue-reader | queue-approver | account-creator |
-----|-------------------|----------------------|--------------|----------------|-----------------|
02358| null              | yes                  | yes          | yes            | yes             |
83950| null              | yes                  | yes          | yes            | no              |
22741| null              | null                 | null         | null           | null            |

Here, some roles have explicit values and some do not. For instance, entry 83950
(associated with UID 87, user `chuges`) has no explicit value (`null`) for
**definition-reader** and explicit values for all the other fields, while entry 22741
(associated with UID 34, user `djanes`) has default values everywhere.

This is valuable because, if a problem with the default settings is discovered, it would
be quite difficult to set the roles for every user created before that point. With this
system, values are only non-`null` if they have been explicitly set; null values resolve
to defaults set elsewhere.

This approach also provides the flexibility I was looking for. If I decide to trust
`djanes` with creating accounts but not, say, approving new definitions, I need only
change the `account-creator` column of the Roles table to `yes` (or `true`, or however
the database stores Boolean values) and that account can now create new accounts.

There is a major issue with this model, however. It does not have a good way to handle
access control for users with no identity, or with an identity I don't know about until
they first interact with us over a federation protocol.

## Anonymous Identity

Anonymous users - those interacting through an unauthenticated web interface and without
identity provided by a federation partner - are actually pretty easy to handle. I already
laid out the need for default permissions for logged-in users, so I can just add another
set of defaults for not-logged-in users (probably just **DefinitionReader**).

## New Introductions

Like other federated systems (such as e-mail), an ActivityPub identity is determined by
both a username (like `ltindall`) and a domain (for instance, `nora.codes`). Assuming
I hosted a FediDict instance on `computerjargon.online` with the user accounts shown
above, their full federated usernames would be `ltindall@computerjargon.online`,
`chughes@computerjargon.online`, and `djanes@computerjargon.online`. These are identities
ComputerJargon.Online already knows about, but the system could get a "like" from a user on
another ActivityPub-supporting instance at any time.

Let's say Eugen, creator of Mastodon, follows the definitions created by users of
ComputerJargon.Online and sees a definition he really likes. He could click the "like"
button in the Mastodon UI, which would eventually notify FediDict that a like had been
sent by `gargron@mastodon.social`. Assuming ComputerJargon.Online had never interacted
with Eugen before, he would not have an account in the ComputerJargon.Online database.
So, how does FediDict determine his access rights?

Defaults come to the rescue again. With a set of defined defaults for accounts coming in
from federation partners, FediDict simply has to create a new record for Eugen's account
in the accounts table (noting its remote origin) with a null access control row, giving
it default values for remote accounts.

# A Holistic View

These considerations alone are not enough to make a system secure, but with good underlying
technologies and a solid access control model, which I'll refine in future posts and as
I begin to write more code, it should be easy to keep egregious bugs out and fix and issues
that do make it into the codebase.

Security comes in layers, and this post has laid out a few of them: one at the very bottom,
the language and underlying technology used to build the system, and one near the top, the
access control model. I can fill in the rest as time goes on.
