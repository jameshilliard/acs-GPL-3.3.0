-- mappings 

-- objectClass mappings: these may be viewed as structuralObjectClass, the ones that are used to decide how to build an entry
--	id		a unique number identifying the objectClass
--	name		the name of the objectClass; it MUST match the name of an objectClass that is loaded in slapd's schema
--	keytbl		the name of the table that is referenced for the primary key of an entry
--	keycol		the name of the column in "keytbl" that contains the primary key of an entry; the pair "keytbl.keycol" uniquely identifies an entry of objectClass "id"
--	create_proc	a procedure to create the entry
--	delete_proc	a procedure to delete the entry; it takes "keytbl.keycol" of the row to be deleted
--	expect_return	a bitmap that marks whether create_proc (1) and delete_proc (2) return a value or not
insert into ldap_oc_mappings (id,name,keytbl,keycol,create_proc,delete_proc,expect_return) values (1,'inetOrgPerson','persons','id','select create_person()','select delete_person(?)',0);

insert into ldap_oc_mappings (id,name,keytbl,keycol,create_proc,delete_proc,expect_return) values (2,'document','documents','id',NULL,NULL,0);

insert into ldap_oc_mappings (id,name,keytbl,keycol,create_proc,delete_proc,expect_return) values (3,'organization','institutes','id',NULL,NULL,0);

-- attributeType mappings: describe how an attributeType for a certain objectClass maps to the SQL data.
--	id		a unique number identifying the attribute	
--	oc_map_id	the value of "ldap_oc_mappings.id" that identifies the objectClass this attributeType is defined for
--	name		the name of the attributeType; it MUST match the name of an attributeType that is loaded in slapd's schema
--	sel_expr	the expression that is used to select this attribute (the "select <sel_expr> from ..." portion)
--	from_tbls	the expression that defines the table(s) this attribute is taken from (the "select ... from <from_tbls> where ..." portion)
--	join_where	the expression that defines the condition to select this attribute (the "select ... where <join_where> ..." portion)
--	add_proc	a procedure to insert the attribute; it takes the value of the attribute that is added, and the "keytbl.keycol" of the entry it is associated to
--	delete_proc	a procedure to delete the attribute; it takes the value of the attribute that is added, and the "keytbl.keycol" of the entry it is associated to
--	param_order	a mask that marks if the "keytbl.keycol" value comes before or after the value in add_proc (1) and delete_proc (2)
--	expect_return	a mask that marks whether add_proc (1) and delete_proc(2) are expected to return a value or not
insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (1,1,'cn','text(persons.name||'' ''||persons.surname)','persons',NULL,'select update_person_cn(?,?)',NULL,3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (2,1,'telephoneNumber','phones.phone','persons,phones','phones.pers_id=persons.id','select add_phone(?,?)','select delete_phone(?,?)',3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (3,1,'givenName','persons.name','persons',NULL,'update persons set name=? where id=?',NULL,3,0);
insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (4,1,'sn','persons.surname','persons',NULL,'update persons set surname=? where id=?',NULL,3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (5,2,'description','documents.abstract','documents',NULL,NULL,NULL,3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (6,2,'documentTitle','documents.title','documents',NULL,NULL,NULL,3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (7,2,'documentAuthor','documentAuthor.dn','ldap_entries AS documentAuthor,documents,authors_docs,persons','documentAuthor.keyval=persons.id AND documentAuthor.oc_map_id=1 AND authors_docs.doc_id=documents.id AND authors_docs.pers_id=persons.id',NULL,NULL,3,0);
	
insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (8,3,'o','institutes.name','institutes',NULL,NULL,NULL,3,0);

insert into ldap_attr_mappings (id,oc_map_id,name,sel_expr,from_tbls,join_where,add_proc,delete_proc,param_order,expect_return) values (9,1,'documentIdentifier','documentIdentifier.dn','ldap_entries AS documentIdentifier,documents,authors_docs,persons','documentIdentifier.keyval=documents.id AND documentIdentifier.oc_map_id=2 AND authors_docs.doc_id=documents.id AND authors_docs.pers_id=persons.id',NULL,NULL,3,0);


-- entries mapping: each entry must appear in this table, with a unique DN rooted at the database naming context
--	id		a unique number > 0 identifying the entry
--	dn		the DN of the entry, in "pretty" form
--	oc_map_id	the "ldap_oc_mappings.id" of the main objectClass of this entry (view it as the structuralObjectClass)
--	parent		the "ldap_entries.id" of the parent of this objectClass; 0 if it is the "suffix" of the database
--	keyval		the value of the "keytbl.keycol" defined for this objectClass
insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (1,'o=sql,c=RU',3,0,1);

insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (2,'cn=Mitya Kovalev,o=sql,c=RU',1,1,1);

insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (3,'cn=Torvlobnor Puzdoy,o=sql,c=RU',1,1,2);

insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (4,'cn=Akakiy Zinberstein,o=sql,c=RU',1,1,3);

insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (5,'documentTitle=book1,o=sql,c=RU',2,1,1);

insert into ldap_entries (id,dn,oc_map_id,parent,keyval) values (6,'documentTitle=book2,o=sql,c=RU',2,1,2);
	
	
-- objectClass mapping: entries that have multiple objectClass instances are listed here with the objectClass name (view them as auxiliary objectClass)
--	entry_id	the "ldap_entries.id" of the entry this objectClass value must be added
--	oc_name		the name of the objectClass; it MUST match the name of an objectClass that is loaded in slapd's schema
insert into ldap_entry_objclasses (entry_id,oc_name) values (4,'referral');

-- referrals mapping: entries that should be treated as referrals are stored here
--	entry_id	the "ldap_entries.id" of the entry that should be treated as a referral
--	url		the URI of the referral
insert into ldap_referrals (entry_id,url) values (4,'ldap://localhost/');

-- procedures
-- these procedures are specific for this RDBMS and are used in mapping objectClass and attributeType creation/modify/deletion
create function create_person () returns int
as '
	select setval (''persons_id_seq'', (select max(id) from persons));
	insert into persons (id,name,surname) 
		values (nextval(''persons_id_seq''),'''','''');
	select max(id) from persons
' language 'sql';

create function update_person_cn (varchar, int) returns int
as '
	update persons set name = (
		select case 
			when position('' '' in $1) = 0 then $1 
			else substr($1, 1, position('' '' in $1) - 1)
		end
	),surname = (
		select case 
			when position('' '' in $1) = 0 then ''''
			else substr($1, position('' '' in $1) + 1) 
		end
	) where id = $2;
	select $2 as return
' language 'sql';

create function delete_person (int) returns int
as '
	delete from phones where pers_id = $1;
	delete from authors_docs where pers_id = $1;
	delete from persons where id = $1;
	select $1 as return
' language 'sql';

create function add_phone (varchar, int) returns int
as '
	select setval (''phones_id_seq'', (select max(id) from phones));
	insert into phones (id,phone,pers_id)
		values (nextval(''phones_id_seq''),$1,$2);
	select max(id) from phones
' language 'sql';

create function delete_phone (varchar, int) returns int
as '
	delete from phones where phone = $1 and pers_id = $2;
	select $2 as result
' language 'sql';

