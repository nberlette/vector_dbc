%%

    /* Version */
"VERSION" {
    return Vector::DBC::Parser::make_VERSION(loc); }

    /* New Symbols */
"NS_" {
    return Vector::DBC::Parser::make_NS(loc); }

    /* Bit Timing */
"BS_:" {
    return Vector::DBC::Parser::make_BS(loc); }

    /* Node */
"BU_" {
    return Vector::DBC::Parser::make_BU(loc); }

    /* Value Table */
"VAL_TABLE_" {
    return Vector::DBC::Parser::make_VAL_TABLE(loc); }

    /* Message */
"BO_" {
    return Vector::DBC::Parser::make_BO(loc); }

    /* Signal */
"SG_" {
    return Vector::DBC::Parser::make_SG(loc); }

    /* Message Transmitters */
"BO_TX_BU_" {
    return Vector::DBC::Parser::make_BO_TX_BU(loc); }

    /* Environment Variable */
"EV_" {
    return Vector::DBC::Parser::make_EV(loc); }

    /* Environment Variable Data */
"ENVVAR_DATA_" {
    return Vector::DBC::Parser::make_ENVVAR_DATA(loc); }

    /* Signal Type */
"SGTYPE_" {
    return Vector::DBC::Parser::make_SGTYPE(loc); }

    /* Comment */
"CM_" {
    return Vector::DBC::Parser::make_CM(loc); }

    /* Attribute Definition */
"BA_DEF_" {
    return Vector::DBC::Parser::make_BA_DEF(loc); }

    /* Attribute Definition at Relation */
"BA_DEF_REL_" {
    return Vector::DBC::Parser::make_BA_DEF_REL(loc); }

    /* Attribute Default */
"BA_DEF_DEF_" {
    return Vector::DBC::Parser::make_BA_DEF_DEF(loc); }

    /* Attribute Default at Relation */
"BA_DEF_DEF_REL_" {
    return Vector::DBC::Parser::make_BA_DEF_DEF_REL(loc); }

    /* Attribute Value */
"BA_" {
    return Vector::DBC::Parser::make_BA(loc); }

    /* Value Description */
"VAL_" {
    return Vector::DBC::Parser::make_VAL(loc); }

    /* Category Definition */
    // CAT_DEF

    /* Categories */
    // CAT

    /* Filter */
    // FILTER

    /* Signal Type Ref */
    // SGTYPE

    /* Signal Group */
"SIG_GROUP_" {
    return Vector::DBC::Parser::make_SIG_GROUP(loc); }

    /* Signal Extended Value Type */
"SIG_VALTYPE_" {
    return Vector::DBC::Parser::make_SIG_VALTYPE(loc); }

    /* Extended Multiplexor */
"SG_MUL_VAL_" {
    return Vector::DBC::Parser::make_SG_MUL_VAL(loc); }

    /* whitespace */
[ \t\v\n\f\r] {
    }

/* yet unmatched characters */
. {
    std::cerr << "unmatched character: " << *yytext; }

    /* match end of file */
<<EOF>> {
    return LIN::NCF::Parser::make_END(loc); }

%%

