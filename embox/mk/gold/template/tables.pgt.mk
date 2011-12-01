##LANGUAGE 'GNU Make'
##ENGINE-NAME 'MyBuild'
##TEMPLATE-NAME 'Parser tables'
##AUTHOR 'Eldar Abusalimov'
##FILE-EXTENSION 'mk'
##NOTES
This template outputs all necessary tables for the parser.
In general you have to name this file the same as base name of the main script
with '-tables' suffix.

You should not modify the generated file.
##END-NOTES
##ID-SEPARATOR '_'
# Generated by GOLD Parser Builder using MyBuild program template. Do not edit!

##PARAMETERS
# Grammar info:
#   Name            : %Name%
#   Version         : %Version%
#   Author          : %Author%
#   About           : %About%
#   Case Sensitive  : %CaseSensitive%
#   Start Symbol    : %StartSymbol%
##END-PARAMETERS

ifndef gold_grammar
$(error 'gold_grammar' is not defined. \
	Do not include $(notdir $(lastword $(MAKEFILE_LIST))) directly!)
endif # gold_grammar

# Contains all necessary tables.
define __gold_$(gold_grammar)_parser
	$(gold-parser \
##SYMBOL-TABLE
		$(gold-symbol-table \
		# 	$(gold-symbol <ID>,
		# 		<Type>,
		# 		<Function name>
		# 	),
##SYMBOLS
			$(gold-symbol %Index%,%Kind%,%ID%),
##END-SYMBOLS
		),# Total %Count% symbol(s).
##END-SYMBOL-TABLE

##RULE-TABLE
		$(gold-rule-table \
		# 	$(gold-rule <ID>,
		# 		<LHS nonterminal ID>,
		# 		<RHS total symbols>,
		# 		<Function name>
		# 	),
##RULES
			$(gold-rule %Index%,%NonterminalIndex%,%SymbolCount%,%ID%),
##END-RULES
		),# Total %Count% rule(s).
##END-RULE-TABLE

##CHAR-SET-TABLE
		$(gold-charset-table \
		# 	$(gold-charset <ID>,
		# 		<List of chars>
		# 	),
##CHAR-SETS
##RANGE-CHARS '.'
##DELIMITER ';'
			$(gold-charset %Index%,%Chars.RangeList%),
##END-CHAR-SETS
		),# Total %Count% charset(s).
##END-CHAR-SET-TABLE

##DFA-TABLE
		$(gold-dfa-table %InitialState%,
		# 	$(gold-dfa-state <ID>,
		# 		<Accept symbol ID or -1>
		# 		$(gold-dfa-edge <Charset ID>,
		# 			<Target state ID>
		# 		),
		# 		...
		# 	),
			$(gold-void \
##DFA-STATES
			),$(gold-dfa-state %Index%,%AcceptIndex%,
##DFA-EDGES
				$(gold-dfa-edge %CharSetIndex%,%Target%),
##END-DFA-EDGES
##END-DFA-STATES 
			)
		),# Total %Count% DFA state(s).
##END-DFA-TABLE

##LALR-TABLE
		$(gold-lalr-table %InitialState%,# <- Initial state
		# 	$(gold-lalr-state <ID>,
		# 		$(gold-lalr-action <Symbol ID>,
		# 			<Action>,
		# 			<Value>
		# 		),
		# 		...
		# 	),
			$(gold-void \
##LALR-STATES
			),$(gold-lalr-state %Index%,
##LALR-ACTIONS
				$(gold-lalr-action %SymbolIndex%,%Action%,%Value%),
##END-LALR-ACTIONS
##END-LALR-STATES
			)
		)# Total %Count% LALR state(s).
##END-LALR-TABLE

	)
endef # __gold_$(gold_grammar)_parser

$(def_all)

