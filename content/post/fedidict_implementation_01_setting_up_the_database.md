---
title: "Fedidict Implementation: Setting Up the Database"
slug: fedidict_implementation_01_setting_up_the_database
date: 2018-09-06T12:31:37-05:00
categories:
- Fedidict
- Programming
- Rust
- Databases
description: The core of Fedidict is the data it stores, for which it uses a traditional SQL database with a simple but powerful schema. This post explores both the SQL and Rust sides of the schema.
---

*This is the first of several posts on the implementation of my ActivityPub-enabled Rust
web application, FediDict. Each of these posts was released to my Patreon patrons before
being make publicly available. I recommend that you read the
[security design post](/post/fedidict_ux_design_part_2/) first.*

The core of any stateful service is its datastore, and FediDict uses PostgreSQL, a very
performant, ACID-compliant, free and open source relational database. In this post, I'll
look at how to make an efficient, normalized database for FediDict's data.

That's a lot of jargon, so let's break it down.

- "Performant", here, means that it is capable of many simultaneous reads, since that is
likely to be the main load.
- "ACID-compliant" basically means that PostgreSQL won't lose data, unless I explicitly
ask it to.
- "Free and open source" is important becuase I want anyone to be able to run FediDict.
- "Relational" means that relations, such as "user x owns definition y", are encoded in
the database itself, rather than being something FediDict has to handle at the application
level. This is good for both performance and security.

# From the Ground Up

FediDict is mosly interested in its list of definitions, and that's where I'll begin with
the database implementation as well. A definition requires a lot of attributes. These will
go in the `Definitions` table of the database.

Definition:

- An arbitrary unique ID value
- The term being defined
- The part of speech being defined (refers to a part of speech; a **foreign key** for PartOfSpeech)
- The actual text of the definition
- A list of related ("see also") terms
- Date of creation
- Created by (foreign key for User)
- Date of approval (it may not exist; we call this **nullable**)
- Approved by (foreign key for User; nullable)

## Normalization

This is a lot of columns for one database table, and furthermore, some of these items
depend on one another. For instance, it's valid to have no approval date AND no approved
by user ID, or to have both an approval date and an approved by user ID, but not one or
the other.

This is not desirable; it's better to have such constraints encoded in the database, and
this is impossible with a single table solution. Furthermore, a bulk approval could
result in multiple records storing data that refers to a single event, which is a
violation of database normalization. To avoid this, I'll split out approval into its own
table.

I'll also replace the two fields for date of approval and approved by user ID with a
single nullable foreign key reference for an Approval. If it's null, the definition
has not been approved; if it has a value, the definition has been approved, and the system
can look up which user approved it.

Definition:

- An arbitrary unique ID value
- The term being defined
- The part of speech being defined (that is, "kick, n." is not the same as "kick, v.")
- The actual text of the definition
- A list of related ("see also") terms
- Date of creation
- Created by (foreign key for User)
- Approval record (nullable, foreign key for Approval)

Written in SQL, the language of the database, that looks like:

```sql
CREATE TABLE Definitions (
    id SERIAL PRIMARY KEY,
    term TEXT NOT NULL,
    part_of_speech INT NOT NULL,
    definition TEXT NOT NULL,
    see_also TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL,
    created_by INT NOT NULL,
    approval INT,
    FOREIGN KEY (part_of_speech) REFERENCES PartsOfSpeech(id),
    FOREIGN KEY (created_by) REFERENCES Users(id),
    FOREIGN KEY (approval) REFERENCES Approvals(id)

);
```

Approval:

- An arbitrary unique ID value
- The user responsible for approving the definition (foreign key for User)
- The date on which the definition was approved

In SQL, this looks like:

```sql
CREATE TABLE Approvals (
    id SERIAL PRIMARY KEY,
    approved_by INT NOT NULL,
    approved_at TIMESTAMP NOT NULL,
    FOREIGN KEY (approved_by) REFERENCES Users(id)
);
```

The final definition-related table is PartsOfSpeech. I considered using a hard-coded list
of parts of speech, but I realized that this isn't scalable across languages, so I will
definitely need to let users define their own. Fortunately, it's very simple.

Part of Speech:

- An arbitrary unique ID value
- The name of the part of speech, like "noun"
- The plural of the name, like "nouns"
- The symbol for the part of speech, like "n."

In SQL:

```sql
CREATE TABLE PartsOfSpeech (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    plural TEXT NOT NULL,
    symbol TEXT NOT NULL
);
```

# Identity Crisis

The other important entity in the system is the user. Users have already been referred to
several times in the schema, but given the discussion in the [previous post](/post/fedidict_ux_design_part_2/), how I refer
to them is pretty important.

User:

- An arbitrary unique ID value
- The user's name
- The user's email
- The hash salt for the user's password
- The hashed password used for login
- The federation partner associated with this user (foreign key for FederationPartner, nullable)
- The date upon which the account was created in FediDict's database
- The RBAC record for the user (foreign key for Roles)

This definition has brought in a new structure, as well: a Federation Partner. That's what
I'm calling the other ActivityPub sites that FediDict will cooperate with. For now, I'll
keep them simple.

FederationPartner:

- An arbitrary unique ID value
- A domain name for the partner
- The date upon which this partner became known to FediDict.

Users with a non-null partner ID will be written as **username**@**partner.domain**, while
users with a null partner ID are just **username**.

Here's how these two look in SQL:

```sql
CREATE TABLE Users (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    email TEXT NOT NULL,
    salt TEXT NOT NULL,
    passhash TEXT NOT NULL,
    partner INT,
    created_at TIMESTAMP NOT NULL,
    roles INT NOT NULL,
    FOREIGN KEY (partner) REFERENCES FederationPartners(id),
    FOREIGN KEY (roles) REFERENCES Roles(id)
);

CREATE TABLE FederationPartners (
    id SERIAL PRIMARY KEY,
    domain TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL
);
```

Finally, I need to define the RBAC table. This is based on the previous post:

```sql
CREATE TABLE Roles (
    id SERIAL PRIMARY KEY,
    definition_reader BOOLEAN,
    definition_submitter BOOLEAN,
    definition_evaluator BOOLEAN,
    definition_remover BOOLEAN,
    queue_reader BOOLEAN,
    queue_approver BOOLEAN,
    queue_rejecter BOOLEAN,
    account_creator BOOLEAN,
    account_remover BOOLEAN,
    account_role_assigner BOOLEAN,
);
```
And that's it!

# Next Steps

Now that the database is defined, I need to actually create it. I'm going to use the
Diesel database mapper for Rust. In essence, Diesel allows me to write Rust code that is
automatically converted into SQL, with all the type safety benefits of Rust code.

## Installing Diesel

Most Rust crates are just a `Cargo.toml` edit away, but Diesel is a bit more complex, as
it also has a CLI tool which I'd like to use. So, I first have to:

```text
$ cargo install diesel_cli
```
Note that this requires both the `mysql` and `postgresql` client libraries be installed
on my system, despite the fact that I'm only using PostgreSQL.

Now I do need to add it to my `Cargo.toml`, along with some other libraries. I'm using
`serde` for serialization and deserialization, along with its code generator from 
`serde_derive` and JSON functionality from `serde_json`. I'm also using `dotenv` to
configure the database with a `.env` file and `chrono` to handle dates and times.

```toml
[package]
name = "fedidict"
version = "0.1.0"
authors = ["Leonora Tindall <nora@nora.codes>"]

[dependencies]
serde = "1"
serde_derive = "1"
serde_json = "1"

diesel = { version ="1", features = ["postgres"] }
dotenv = "0.13"
chrono = "0.4"
```

## Installing PostgreSQL

I'm going to use Docker to handle the database.

```bash
docker create -p 127.0.0.1:5432:5432 \
    --name fedidict-db \
    -e POSTGRES_PASSWORD="password" \
    -t postgres:alpine

docker start fedidict-db
```

And, in a file called `.env` in the project folder, I'll make an environment variable:

```
DATABASE_URL="postgresql://postgres:password@localhost/fedidict"
```

and then run `source .env` to get this variable in my shell.

Now I can use Diesel to set up the database. This creates both the database and a
directory to hold SQL files for database setup.

```bash
diesel setup
```

## Heading North

Diesel's CLI tool helps manage database state, and in fact I'll use it to create the
database to begin with.

I'll add a new **migration** using `diesel_cli` and put some SQL code in it telling
Diesel how to set up (and tear down) the database. This is the only SQL I expect to write,
since Diesel will handle interaction with the database in most cases.

```text
$ diesel migration generate initial_schema
Creating migrations/2018-09-10-170446_initial_schema/up.sql
Creating migrations/2018-09-10-170446_initial_schema/down.sql
```

In Diesel, migrations go both "up" and "down"; up migrations add new functionality, while
down migrations remove that functionality.

The up migration is the combination of the SQL I've written so far in the post, but the
order is important; tables must be created in dependency order. For instance, Approvals
must be created before Definitions because Definitions has a foreign key that points
to Approvals.

The order I decided on is PartsOfSpeech, FederationPartners, Roles, Users, Approvals,
Definitions.

The down migration is much simpler. I simply drop all the tables I created in tue up
migration.

```sql
DROP TABLE Roles, Definitions, Approvals, PartsOfSpeech, Users, FederationPartners;
```

It's good practice to test migrations by running them and then rerunning them.

```bash
$ diesel migration run                                   
Running migration 2018-09-10-170446_initial_schema
$ diesel migration redo 
Rolling back migration 2018-09-10-170446_initial_schema
Running migration 2018-09-10-170446_initial_schema
```

This verifies that the down migration at least doesn't leave anything obvious hanging
around.

After running this migration, two new files will show up: `diesel.toml`, Diesel's config
file, and `src/schema.rs`, the Rust mapping of the database schema.

That's it - the database is set up. In the next post, I'll discuss how I map these
data structures into Rust, how I test them, and what behavior I need to set up for them.
