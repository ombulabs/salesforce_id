#include "salesforce_id_ext.h"
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "repair_casing.h"
#include "insensitive_to_sensitive.h"
#include "sensitive_to_insensitive.h"
#include "validate_id.h"

VALUE rb_mSalesforceId;

static VALUE VALID_CHARACTERS_to_rb();

void Init_salesforce_id()
{
  // Module
  rb_mSalesforceId = rb_define_module("SalesforceId");

  // Constants
  rb_define_const(
    rb_mSalesforceId,
    "SENSITIVE_SIZE",
    INT2FIX(SALESFORCE_ID_SENSITIVE_LENGTH)
  );
  rb_define_const(
    rb_mSalesforceId,
    "INSENSITIVE_SIZE",
    INT2FIX(SALESFORCE_ID_INSENSITIVE_LENGTH)
  );
  rb_define_const(
    rb_mSalesforceId,
    "VALID_CHARACTERS",
    VALID_CHARACTERS_to_rb()
  );

  // Methods
  rb_define_method(
    rb_mSalesforceId,
    "to_sensitive",
    salesforce_id_to_sensitive,
    1
  );
  rb_define_method(
    rb_mSalesforceId,
    "to_insensitive",
    salesforce_id_to_insensitive,
    1
  );
  rb_define_method(
    rb_mSalesforceId,
    "valid?",
    salesforce_id_is_valid,
    1
  );
  rb_define_method(
    rb_mSalesforceId,
    "repair_casing",
    salesforce_insensitive_repair_casing,
    1
  );
  rb_define_method(
    rb_mSalesforceId,
    "sensitive?",
    salesforce_id_is_sensitive,
    1
  );
  rb_define_method(
    rb_mSalesforceId,
    "insensitive?",
    salesforce_id_is_insensitive,
    1
  );
}

// Convert to 15 character case-sensitive id
VALUE salesforce_id_to_sensitive(VALUE self, VALUE rb_sId)
{
  VALUE id = rb_obj_as_string(rb_sId);

  if (!is_id_valid(id))
  {
    rb_raise(rb_eArgError, "Invalid Salesforce ID");
    return Qnil;
  }

  if (RSTRING_LEN(id) == SALESFORCE_ID_SENSITIVE_LENGTH) return id;

  return insensitive_to_sensitive(id);
}

// Convert to 18 character case-insensitive id
VALUE salesforce_id_to_insensitive(VALUE self, VALUE rb_sId)
{
  VALUE id = rb_obj_as_string(rb_sId);

  if (!is_id_valid(id))
  {
    rb_raise(rb_eArgError, "Invalid Salesforce ID");
    return Qnil;
  }

  if (RSTRING_LEN(id) == SALESFORCE_ID_INSENSITIVE_LENGTH) return id;

  return sensitive_to_insensitive(id);
}

VALUE salesforce_id_is_valid(VALUE self, VALUE rb_sId)
{
  VALUE id = rb_obj_as_string(rb_sId);

  if (is_id_valid(id)) return Qtrue;

  return Qfalse;
}

VALUE salesforce_insensitive_repair_casing(VALUE self, VALUE rb_sId)
{
  // Start for insensitive characters
  const size_t istart          = (size_t)SALESFORCE_ID_SENSITIVE_LENGTH;
  const size_t sensitive_chars = 3u;
  const int    new_id_size     = SALESFORCE_ID_INSENSITIVE_STRING_LENGTH;
        char*  old_id          = NULL;
        VALUE  id              = Qnil;
        char   new_id[SALESFORCE_ID_INSENSITIVE_STRING_LENGTH] = {0};

  id     = rb_obj_as_string(rb_sId);
  old_id = StringValueCStr(id);

  if (!is_id_valid(id) || RSTRING_LEN(id) != SALESFORCE_ID_INSENSITIVE_LENGTH)
    rb_raise(rb_eArgError, "Salesforce ID is not case-insensitive format");

  memcpy(new_id, old_id, new_id_size);
  repair_casing(new_id);
  memcpy(&new_id[istart], &old_id[istart], sizeof(new_id[0]) * sensitive_chars);

  for (size_t index = 0u; index < sensitive_chars; ++index)
    new_id[istart + index] = toupper(new_id[istart + index]);

  return rb_str_new2(new_id);
}

VALUE salesforce_id_is_sensitive(VALUE self, VALUE rb_sId)
{
  VALUE id = rb_obj_as_string(rb_sId);

  if (is_id_valid(id) && (RSTRING_LEN(id) == SALESFORCE_ID_SENSITIVE_LENGTH))
    return Qtrue;

  return Qfalse;
}

VALUE salesforce_id_is_insensitive(VALUE self, VALUE rb_sId)
{
  VALUE id = rb_obj_as_string(rb_sId);

  if (is_id_valid(id) && (RSTRING_LEN(id) == SALESFORCE_ID_INSENSITIVE_LENGTH))
    return Qtrue;

  return Qfalse;
}

VALUE VALID_CHARACTERS_to_rb()
{
  VALUE rb_Chars = Qnil;

  rb_Chars = rb_ary_new2(VALID_CHARMAP_SIZE);

  for (long index = 0; index < VALID_CHARMAP_SIZE; ++index)
  {
    char  str_Char[2];
    VALUE rb_Char;

    memset(&str_Char[1], 0, sizeof(str_Char[1]));
    memset(str_Char, VALID_CHARMAP[index], sizeof(str_Char[0]));

    rb_Char = rb_str_new2(str_Char);

    rb_ary_store(rb_Chars, index, rb_obj_freeze(rb_Char));
  }

  return rb_ary_freeze(rb_Chars);
}
