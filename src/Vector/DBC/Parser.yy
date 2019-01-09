%token VERSION
%type <> version

%token NS
%type <> new_symbols

%token BS
%type <Message> message

%token BU
%type <Node> node

%token VAL_TABLE
%type <ValueTable> value_table

%token BO
%type <Message> message

%token SG
%type <Signal> signal

%token BO_TX_BU
%type <> message_transmitter

%token EV
%type <EnvironmentVariable> environment_variable

%token ENVVAR_DATA
%type <> environment_variable_data

%token SGTYPE
%type <> signal_type

%token CM
%type <> comment

%token BA_DEF
%type <AttributeDefinition> attribute_definition

%token BA_DEF_REL
%type <> attribute_definition_relation

%token BA_DEF_DEF
%type <> attribute_default

%token BA_DEF_DEF_REL
%type <> attribute_default_relation

%token BA
%type <> attribute_value

%token BA_REL
%type <> attribute_relation_value

%token VAL
%type <> value_description

%token CAT_DEF

%token CAT

%token FILTER

%token SGTYPE

%token SIG_GROUP
%type <SignalGroup> signal_group

%token SIG_VALTYPE
%type <> signal_extended_value_type

%token SG_MUL_VAL
%type <ExtendedMultiplexor> extended_multiplexor

%token END 0

%%

version
        : VERSION string { }
        ;

new_symbols
        : NS {}
        ;

bit_timing
        : BS uint COLON uint COLON uint {}
        ;

nodes
        : BU
        ;

value_table
        : VAL_TABLE
        ;

signal
        : BO
        ;

message
        : SG
        ;

message_transmitter
        : BO_TX_BU
        ;

environment_variable
        : EV
        ;

environment_variable_data
        : ENVVAR_DATA
        ;

signal_type
        : SGTYPE
        ;

comment_network
        : CM
        ;

comment_node
        : CM
        ;

comment_message
        : CM
        ;

comment_signal
        : CM
        ;

comment_environment_variable
        : CM
        ;

comment
        : CM
        ;

attribute_definition
        : BA_DEF
        ;

attribute_definition_relation
        : BA_DEF_REL
        ;

attribute_default
        : BA_DEF_DEF
        ;

attribute_default_relation
        : BA_DEF_DEF_REL
        ;

attribute_value_network
        : BA
        ;

attribute_value_node
        : BA
        ;

attribute_value_message
        : BA
        ;

attribute_value_signal
        : BA
        ;

attribute_value_environment_variable
        : BA
        ;

attribute_value
        : BA
        ;

attribute_relation_value
        : BA_REL
        ;

value_description_signal
        : VAL
        ;

value_description_environment_variable
        : VAL
        ;

value_description
        : VAL
        ;

signal_group
        : SIG_GROUP
        ;

signal_extended_value_type
        : SIG_VALTYPE
        ;

extended_multiplexor
        : SG_MUL_VAL
        ;
