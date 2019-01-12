%skeleton "lalr1.cc"
%require "3.0"
%defines
%define api.namespace {Vector::DBC}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.trace
%define parser_class_name {Parser}
%language "C++"
%locations

    // debug options
%verbose
%debug
%error-verbose

%code requires{
#include <Vector/DBC/Network.h>
namespace Vector {
namespace DBC {
class Network;
}
}
}

%lex-param { const Vector::DBC::Parser::location_type & loc }
%parse-param { class Scanner * scanner }
%parse-param { class Network * network }

%code{
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <Vector/DBC/Network.h>
#include <Vector/DBC/Scanner.h>

#undef yylex
#define yylex scanner->yylex

#define loc scanner->location
}

    // %destructor { delete($$); ($$) = nullptr; } <network>

    /* 2 General Definitions */
%token <std::string> UNSIGNED_INTEGER SIGNED_INTEGER DOUBLE CHAR_STRING DBC_IDENTIFIER
%type <uint32_t> unsigned_integer
%type <int32_t> signed_integer
%type <double> double
%type <std::string> char_string
%type <std::vector<std::string>> char_strings
%type <std::string> dbc_identifier

    /* 3 Structure of the DBC File */
//%type <File> file

    /* 4 Version and New Symbol Specification */
%token VERSION NS_START NS_END
//%type <std::string> version
//%type <std::string> candb_version_string
//%type <std::vector<std::string>> new_symbols
//%type <std::vector<std::string>> new_symbol_values

    /* 5 Bit Timing Definition */
%token BS
//%type <BitTiming> bit_timing
//%type <uint32_t> baudrate
//%type <uint32_t> btr1
//%type <uint32_t> btr2

    /* 6 Node Definitions */
%token BU
//%type <std::set<std::string>> nodes
//%type <std::set<std::string>> node_names
//%type <std::string> node_name

    /* 7 Value Table Definitions */
%token VAL_TABLE
//%type <std::map<std::string, ValueTable>> value_tables
//%type <ValueTable> value_table
//%type <std::string> value_table_name

    /* 7.1 Value Descriptions (Value Encodings) */
//%type <std::map<uint32_t, std::string>> value_encoding_descriptions
//%type <std::pair<uint32_t, std::string>> value_encoding_description

    /* 8 Message Definitions */
%token BO VECTOR_XXX
//%type <std::map<uint32_t, Message>> messages
//%type <Message> message
//%type <uint32_t> message_id
//%type <std::string> message_name
//%type <uint32_t> message_size
//%type <std::string> transmitter

    /* 8.1 Pseudo-message */

    /* 8.2 Signal Definitions */
%token SG LOWER_M UPPER_M SIG_VALTYPE
//%type <std::map<std::string, Signal>> signals
//%type <Signal> signal
//%type <std::string> signal_name
//%type <std::vector<std::string>> signal_names
//%type <MultiplexerIndicator> multiplexer_indicator
//%type <uint32_t> multiplexer_switch_value
//%type <uint32_t> start_bit
//%type <uint32_t> signal_size
//%type <char> byte_order
//%type <char> value_type
//%type <double> factor
//%type <double> offset
//%type <double> minimum
//%type <double> maximum
//%type <std::string> unit
//%type <std::vector<std::string>> receivers
//%type <std::string> receiver
//%type <SignalExtendedValueTypeList> signal_extended_value_type_list
//%type <char> signal_extended_value_type

    /* 8.3 Definition of Message Transmitters */
%token BO_TX_BU
//%type <std::map<uint32_t, MessageTransmitter>> message_transmitters
//%type <MessageTransmitter> message_transmitter
//%type <std::vector<std::string>> transmitters

    /* 8.4 Signal Value Descriptions (Value Encodings) */
%token VAL
//%type <ValueDescriptions> value_descriptions
//%type <ValueDescriptionsForSignal> value_descriptions_for_signal

    /* 9 Environment Variable Definitions */
%token EV
%token DUMMY_NODE_VECTOR0
%token DUMMY_NODE_VECTOR1
%token DUMMY_NODE_VECTOR2
%token DUMMY_NODE_VECTOR3
%token DUMMY_NODE_VECTOR8000
%token DUMMY_NODE_VECTOR8001
%token DUMMY_NODE_VECTOR8002
%token DUMMY_NODE_VECTOR8003
%token ENVVAR_DATA
//%type <std::map<std::string, EnvironmentVariable>> environment_variables
//%type <EnvironmentVariable> environment_variable
//%type <std::string> env_var_name
//%type <char> env_var_type
//%type <double> initial_value
//%type <uint32_t> ev_id
//%type <uint16_t> access_type
//%type <std::vector<std::string>> access_nodes
//%type <std::string> access_node
//%type <std::map<std::string, EnvironmentVariableData>> environment_variables_data
//%type <EnvironmentVariableData> environment_variable_data
//%type <uint32_t> data_size

    /* 9.1 Environment Variable Value Descriptions */
//%type <ValueDescriptionsForEnvVar> value_descriptions_for_env_var

    /* 10 Signal Type and Signal Group Definitions */
%token SGTYPE SIG_GROUP
//%type <std::map<std::string, SignalType>> signal_types
//%type <SignalType> signal_type
//%type <std::string> signal_type_name
//%type <double> default_value
//%type <std::vector<SignalTypeRef>> signal_type_refs
//%type <SignalTypeRef> signal_type_ref
//%type <SignalGroups> signal_groups
//%type <std::string> signal_group_name
//%type <uint32_t> repetitions

    /* 11 Comment Definitions */
%token CM
//%type <std::vector<Comment>> comments
//%type <Comment> comment

    /* 12 User Defined Attribute Definitions */

    /* 12.1 Attribute Definitions */
%token BA_DEF INT HEX FLOAT STRING ENUM
%token BA_DEF_REL BU_EV_REL BU_BO_REL BU_SG_REL
//%type <std::vector<AttributeDefinition>> attribute_definitions
//%type <AttributeDefinition> attribute_definition
//%type <uint8_t> object_type
//%type <std::string> attribute_name
//%type <AttributeValueType> attribute_value_type

    /* Attribute Defaults */
%token BA_DEF_DEF
%token BA_DEF_DEF_REL
//%type <std::vector<AttributeDefault>> attribute_defaults
//%type <AttributeDefault> attribute_default
//%type <AttributeValue> attribute_value

    /* 12.2 Attribute Values */
%token BA
%token BA_REL
//%type <std::vector<AttributeValueForObject>> attribute_values
//%type <void *> attribute_value_for_object

    /* 13 Extended Multiplexing */
%token SG_MUL_VAL
//%type <std::vector<MultiplexedSignal>> extended_multiplexing
//%type <MultiplexedSignal> multiplexed_signal
//%type <std::string> multiplexed_signal_name
//%type <std::string> multiplexor_switch_name
//%type <std::vector<MultiplexorValueRange>> multiplexor_value_ranges
//%type <MultiplexorValueRange> multiplexor_value_range

    /* Punctuators */
%token OPEN_BRACKET CLOSE_BRACKET OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token PLUS MINUS VERTICAL_BAR COLON SEMICOLON ASSIGN COMMA AT

    /* end of line */
%token EOL

    /* match end of file */
%token END 0

%%

    /* 3 Structure of the DBC File */
file
        : version                           // VERSION
          new_symbols                       // NS_
          bit_timing                        // BS_
          nodes                             // BU_
          value_tables                      // VAL_TABLE_
          messages                          // BO_ / SG_
          message_transmitters              // BO_TX_BU_
          environment_variables             // EV_
          environment_variables_data        // ENVVAR_DATA_
          signal_types                      // SGTYPE_
          comments                          // CM_ ...
          attribute_definitions             // BA_DEF_(REL_)
          attribute_defaults                // BA_DEF_DEF_(REL_)
          attribute_values                  // BA_(REL_)
          value_descriptions                // VAL_
          signal_groups                     // SIG_GROUP_
          signal_extended_value_types       // SIG_VALTYPE_
          extended_multiplexing {}          // SG_MUL_VAL
        ;

    /* 2 General Definitions */
unsigned_integer
        : UNSIGNED_INTEGER { $$ = std::stoul($1); }
        ;
signed_integer
        : SIGNED_INTEGER { $$ = std::stol($1); }
        | UNSIGNED_INTEGER { $$ = std::stol($1); }
        ;
double
        : DOUBLE { $$ = std::stod($1); }
        | SIGNED_INTEGER { $$ = std::stod($1); }
        | UNSIGNED_INTEGER { $$ = std::stod($1); }
        ;
char_string
        : CHAR_STRING { $$ = $1; }
        ;
char_strings
        : CHAR_STRING { $$ = std::vector<std::string>(); $$.push_back($1); }
        | char_strings COMMA CHAR_STRING { $$ = $1; $$.push_back($3); }
        ;
dbc_identifier
        : DBC_IDENTIFIER { $$ = $1; }
        ;

    /* 4 Version and New Symbol Specification */
version
        : VERSION candb_version_string EOL /* { $$ = $2; } */
        ;
candb_version_string
        : char_string /* { $$ = $1; } */
        ;
new_symbols
        : %empty
        | NS_START
          new_symbol_values /* { $$ = $3; } */
          NS_END
        ;
new_symbol_values
        : %empty /* { $$ = std::vector<std::string>(); } */
        | new_symbol_values DBC_IDENTIFIER /* { $$ = $1; $$.push_back($2); } */
        ;

    /* 5 Bit Timing Definition */
bit_timing
        : BS COLON EOL /* { $$ = BitTiming(); } */
        | BS COLON baudrate COLON btr1 COMMA btr2 EOL /* { $$ = BitTiming($2, $4, $6); } */
        ;
baudrate
        : unsigned_integer /* { $$ = $1; } */
        ;
btr1
        : unsigned_integer /* { $$ = $1; } */
        ;
btr2
        : unsigned_integer /* { $$ = $1; } */
        ;

    /* 6 Node Definitions */
nodes
        : BU COLON node_names EOL /* { $$ = $2; } */
        ;
node_names
        : %empty /* { $$ = std::vector<std::string>(); } */
        | node_names node_name /* { $$ = $1; $$.push_back($2); } */
        ;
node_name
        : dbc_identifier /* { $$ = $1; } */
        ;

    /* 7 Value Table Definitions */
value_tables
        : %empty /* { $$ = std::map<std::string, ValueTable>(); } */
        | value_tables value_table /* { $$ = $1; $$[$2.name] = $2; } */
        ;
value_table
        : VAL_TABLE value_table_name value_encoding_descriptions SEMICOLON EOL /* { $$ = ValueTable($2, $3); } */
        ;
value_table_name
        : dbc_identifier /* { $$ = $1; } */
        ;

    /* 7.1 Value Descriptions (Value Encodings) */
value_encoding_descriptions
        : %empty /* { std::map<uint32_t, std::string>(); } */
        | value_encoding_descriptions value_encoding_description /* { $$ = $1; $$.insert($2); } */
        ;
value_encoding_description
        : unsigned_integer char_string /* { $$ = std::pair<uint32_t, std::string>($1, $2); } */;
        ;

    /* 8 Message Definitions */
messages
        : %empty /* { $$ = std::map<uint32_t, Message>(); } */
        | messages message /* { $$ = $1; $$[$2.id] = $2; } */
        ;
message
        : BO message_id message_name COLON message_size transmitter EOL signals /* { $$ = Message($2, $3, $5, $6, $7); } */
        ;
message_id
        : unsigned_integer /* { $$ = $1; } */
        ;
message_name
        : dbc_identifier /* { $$ = $1; } */
        ;
message_size
        : unsigned_integer /* { $$ = $1; } */
        ;
transmitter
        : node_name /* { $$ = $1; } */
        | VECTOR_XXX /* { $$ = $1; } */
        ;

    /* 8.1 Pseudo-message */

    /* 8.2 Signal Definitions */
signals
        : %empty /* { $$ = std::map<std::string, Signal>(); } */
        | signals signal /* { $$ = $1; $$[$2.name] = $2; } */
        ;
signal
        : SG signal_name multiplexer_indicator COLON start_bit VERTICAL_BAR signal_size AT byte_order value_type OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit receivers EOL /* { $$ = Signal($2, $3, $5, $7, $9, $10, $12, $14, $17, $19, $21, $22); } */
        ;
signal_name
        : dbc_identifier /* { $$ = $1; } */
        ;
signal_names
        : %empty /* { $$ = std::vector<std::string>(); } */
        | signal_names signal_name /* { $$ = $1; $$.push_back($2); } */
        ;
multiplexer_indicator
        : %empty
        | DBC_IDENTIFIER /*LOWER_M multiplexer_switch_value*/ /* { $$ = MultiplexerIndicator(2, $2); } */
        | UPPER_M /* { $$ = MultiplexerIndicator(3); } */
        ;
multiplexer_switch_value
        : unsigned_integer /* { $$ = $1; } */
        ;
start_bit
        : unsigned_integer /* { $$ = $1; } */
        ;
signal_size
        : unsigned_integer /* { $$ = $1; } */
        ;
byte_order
        : UNSIGNED_INTEGER /* { $$ = $1; } */
        ;
value_type
        : PLUS /* { $$ = '+'; } */
        | MINUS /* { $$ = '-'; } */
        ;
factor
        : double /* { $$ = $1; } */
        ;
offset
        : double /* { $$ = $1; } */
        ;
minimum
        : double /* { $$ = $1; } */
        ;
maximum
        : double /* { $$ = $1; } */
        ;
unit
        : char_string /* { $$ = $1; } */
        ;
receivers
        : receiver /* { $$ = std::vector<std::string>(); } */
        | receivers COMMA receiver /* { $$ = $1; $$.push_back($3); } */
        ;
receiver
        : node_name /* { $$ = $1; } */
        /* | VECTOR__XXX { $$ = $1; } */
        ;
signal_extended_value_types
        : %empty
        | signal_extended_value_types signal_extended_value_type
        ;
signal_extended_value_type
        : SIG_VALTYPE message_id signal_name COLON UNSIGNED_INTEGER SEMICOLON EOL /* { $$ = SignalExtendedValueTypeList($2, $3, $4); } */
        ;

    /* 8.3 Definition of Message Transmitters */
message_transmitters
        : %empty /* { $$ = std::map<uint32_t, MessageTransmitter>(); } */
        | message_transmitters message_transmitter /* { $$ = $1; $$[$2.id] = $2; } */
        ;
message_transmitter
        : BO_TX_BU message_id COLON transmitters SEMICOLON EOL /* { $$ = MessageTransmitter($2, $4); } */
        ;
transmitters
        : %empty /* { $$ = std::vector<std::string>(); } */
        | transmitters transmitter /* { $$ = $1; $$.push_back($2); } */
        ;

    /* 8.4 Signal Value Descriptions (Value Encodings) */
value_descriptions
        : %empty {}
        | value_descriptions value_descriptions_for_signal /* { $$ = ValueDescriptions(1, $1); } */
        | value_descriptions value_descriptions_for_env_var /* { $$ = ValueDescriptions(2, $1); } */
        ;
value_descriptions_for_signal
        : VAL message_id signal_name value_encoding_descriptions SEMICOLON EOL /* { $$ = ValueDescriptionsForSignal($2, $3, $4); } */
        ;

    /* 9 Environment Variable Definitions */
environment_variables
        : %empty /* { $$ = std::map<std::string, EnvironmentVariable>(); } */
        | environment_variables environment_variable /* { $$ = $1; $$[$2.name] = $2; } */
        ;
environment_variable
        : EV env_var_name COLON env_var_type OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET unit initial_value ev_id access_type access_nodes SEMICOLON EOL /* { $$ = EnvironmentVariable($2, $4, $6, $8, $10, $11, $12, $13, $14); } */
        ;
env_var_name
        : dbc_identifier /* { $$ = $1; } */
        ;
env_var_type
        : UNSIGNED_INTEGER /* { $$ = '0'; } */
        ;
initial_value
        : double /* { $$ = $1; } */
        ;
ev_id
        : unsigned_integer /* { $$ = $1; } */
        ;
access_type
        : DUMMY_NODE_VECTOR0 /* { $$ = 0x0000; } */
        | DUMMY_NODE_VECTOR1 /* { $$ = 0x0001; } */
        | DUMMY_NODE_VECTOR2 /* { $$ = 0x0002; } */
        | DUMMY_NODE_VECTOR3 /* { $$ = 0x0003; } */
        | DUMMY_NODE_VECTOR8000 /* { $$ = 0x8000; } */
        | DUMMY_NODE_VECTOR8001 /* { $$ = 0x8001; } */
        | DUMMY_NODE_VECTOR8002 /* { $$ = 0x8002; } */
        | DUMMY_NODE_VECTOR8003 /* { $$ = 0x8003; } */
        ;
access_nodes
        : access_node /* { $$ = std::vector<std::string>(); } */
        | access_nodes COMMA access_node /* { $$ = $1; $$.push_back($3); } */
        ;
access_node
        : node_name /* { $$ = $1; } */
        /* | VECTOR__XXX { $$ = $1; } */
        ;
environment_variables_data
        : %empty /* { $$ = std::map<std::string, EnvironmentVariableData>(); } */
        | environment_variables_data environment_variable_data /* { $$ = $1; $$[$2.name] = $2; } */
        ;
environment_variable_data
        : ENVVAR_DATA env_var_name COLON data_size SEMICOLON EOL /* { $$ = EnvironmentVariableData($2, $4); } */
        ;
data_size
        : unsigned_integer /* { $$ = $1; } */
        ;

    /* 9.1 Environment Variable Value Descriptions */
value_descriptions_for_env_var
        : VAL env_var_name value_encoding_descriptions SEMICOLON EOL /* { $$ = ValueDescriptionsForEnvVar($2, $3); } */
        ;

    /* 10 Signal Type and Signal Group Definitions */
signal_types
        : %empty /* { $$ = std::map<std::string, SignalType>(); } */
        | signal_types signal_type /* { $$ = $1; $$[$2.name] = $2; } */
        ;
signal_type
        : SGTYPE signal_type_name COLON signal_size VERTICAL_BAR byte_order value_type
          OPEN_PARENTHESIS factor COMMA offset CLOSE_PARENTHESIS
          OPEN_BRACKET minimum VERTICAL_BAR maximum CLOSE_BRACKET
          unit default_value COMMA value_table_name SEMICOLON EOL /* { $$ = SignalType($2, $4, $6, $7, $9, $11, $14, $16, $18, $19, $21); } */
        ;
signal_type_name
        : dbc_identifier /* { $$ = $1; } */
        ;
default_value
        : double /* { $$ = $1; } */
        ;
signal_type_refs
        : %empty /* { $$ = std::vector<SignalTypeRef>(); } */
        | signal_type_refs signal_type_ref /* { $$ = $1; $$.push_back($2); } */
        ;
signal_type_ref
        : SGTYPE message_id signal_name COLON signal_type_name SEMICOLON EOL /* { $$ = SignalTypeRef($2, $3, $5); } */
        ;
signal_groups
        : %empty
        | signal_groups signal_group
        ;
signal_group
        : SIG_GROUP message_id signal_group_name repetitions COLON signal_names SEMICOLON EOL /* { $$ = SignalGroup($2, $3, $4, $6); } */
        ;
signal_group_name
        : dbc_identifier /* { $$ = $1; } */
        ;
repetitions
        : unsigned_integer /* { $$ = $1; } */
        ;

    /* 11 Comment Definitions */
comments
        : %empty /* { $$ = std::vector<Comment>(); } */
        | comments comment /* { $$ = $1; $$.push_back($2); } */
        ;
comment
        : CM char_string SEMICOLON EOL /* { $$ = Comment(1, $2); } */
        | CM BU node_name char_string SEMICOLON EOL /* { $$ = Comment(2, $2, $3); } */
        | CM BO message_id char_string SEMICOLON EOL /* { $$ = Comment(3, $2, $3); } */
        | CM SG message_id signal_name char_string SEMICOLON EOL /* { $$ = Comment(4, $2, $3, $4); } */
        | CM EV env_var_name char_string SEMICOLON EOL /* { $$ = Comment(5, $2, $3); } */
        ;

    /* 12 User Defined Attribute Definitions */

    /* 12.1 Attribute Definitions */
attribute_definitions
        : %empty /* { $$ = std::vector<AttributeDefinition>(); } */
        | attribute_definitions attribute_definition /* { $$ = $1; $$.push_back($2); } */
        ;
attribute_definition
        : BA_DEF object_type attribute_name attribute_value_type SEMICOLON EOL /* { $$ = AttributeDefinition($2, $3, $4); } */
        | BA_DEF_REL BU_EV_REL attribute_name attribute_value_type SEMICOLON EOL {}
        | BA_DEF_REL BU_BO_REL attribute_name attribute_value_type SEMICOLON EOL {}
        | BA_DEF_REL BU_SG_REL attribute_name attribute_value_type SEMICOLON EOL {}
        ;
object_type
        : %empty /* { $$ = 1; } */
        | BU /* { $$ = 2; } */
        | BO /* { $$ = 3; } */
        | SG /* { $$ = 4; } */
        | EV /* { $$ = 5; } */
        ;
attribute_name
        : char_string /* { $$ = $1; } */
        ;
attribute_value_type
        : INT signed_integer signed_integer /* { $$ = AttributeValueType(1, $2, $3); } */
        | HEX signed_integer signed_integer /* { $$ = AttributeValueType(2, $2, $3); } */
        | FLOAT double double /* { $$ = AttributeValueType(3, $2, $3); } */
        | STRING /* { $$ = AttributeValueType(4); } */
        | ENUM char_strings /* { $$ = AttributeValueType(5, $2); } */
        ;

    /* Attribute Defaults */
attribute_defaults
        : %empty /* { $$ = std::vector<AttributeDefault>(); } */
        | attribute_defaults attribute_default /* { $$ = $1; $$.push_back($2); } */
        ;
attribute_default
        : BA_DEF_DEF attribute_name attribute_value SEMICOLON EOL /* { $$ = AttributeDefault($2, $3); } */
        | BA_DEF_DEF_REL attribute_name attribute_value SEMICOLON EOL/* @todo */
        ;
attribute_value
        : unsigned_integer /* { $$ = AttributeValue(1, $1); } */
        | signed_integer /* { $$ = AttributeValue(2, $1); } */
        | double /* { $$ = AttributeValue(3, $1); } */
        | char_string /* { $$ = AttributeValue(4, $1); } */
        ;

    /* 12.2 Attribute Values */
attribute_values
        : %empty /* { $$ = std::vector<AttributeValueForObject>(); } */
        | attribute_values attribute_value_for_object /* { $$ = $1; $$.push_back($2); } */
        ;
attribute_value_for_object
        : BA attribute_name attribute_value SEMICOLON EOL /* { $$ = AttributeValueForObject(1, $2, $3); } */
        | BA attribute_name BU node_name attribute_value SEMICOLON EOL /* { $$ = AttributeValueForObject(2, $2, $4, $5); } */
        | BA attribute_name BO message_id attribute_value SEMICOLON EOL /* { $$ = AttributeValueForObject(3, $2, $4, $5); } */
        | BA attribute_name SG message_id signal_name attribute_value SEMICOLON EOL /* { $$ = AttributeValueForObject(4, $2, $4, $5, $6); } */
        | BA attribute_name EV env_var_name attribute_value SEMICOLON EOL /* { $$ = AttributeValueForObject(5, $2, $4, $5); } */
        | BA_REL attribute_name BU_EV_REL node_name env_var_name attribute_value SEMICOLON EOL
        | BA_REL attribute_name BU_BO_REL node_name message_id attribute_value SEMICOLON EOL
        | BA_REL attribute_name BU_SG_REL node_name SG message_id signal_name attribute_value SEMICOLON EOL
        ;

    /* 13 Extended Multiplexing */
extended_multiplexing
        : %empty /* { $$ = std::vector<MultiplexedSignal>(); } */
        | extended_multiplexing multiplexed_signal /* { $$ = $1; $$.push_back($2); } */
        ;
multiplexed_signal
        : SG_MUL_VAL message_id multiplexed_signal_name multiplexor_switch_name multiplexor_value_ranges SEMICOLON EOL /* { $$ = MultiplexedSignal($2, $3, $4, $5); } */
        ;
multiplexed_signal_name
        : dbc_identifier /* { $$ = $1; } */
        ;
multiplexor_switch_name
        : dbc_identifier /* { $$ = $1; } */
        ;
multiplexor_value_ranges
        : %empty /* { $$ = std::vector<MultiplexorValueRange>(); } */
        | multiplexor_value_ranges multiplexor_value_range /* { $$ = $1; $$.push_back($2); } */
        ;
multiplexor_value_range
        : unsigned_integer MINUS unsigned_integer /* { $$ = MultiplexorValueRange($1, $3); } */
        ;

%%

void Vector::DBC::Parser::error(const location_type & location, const std::string & message)
{
    std::cerr << "Parse error at " << location << ": " << message << std::endl;
}
