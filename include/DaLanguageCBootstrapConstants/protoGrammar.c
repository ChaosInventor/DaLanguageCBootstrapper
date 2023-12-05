List names = {0};
List grammar = grammar(
// Name = ( [A-z] | [0-9] | '_' ) * 1-
rule(
    (
     ruleOnlyNonterminalByName(&names, "Name"),
     ruleEnd
    ),
    (
     ruleDefinition(
         (
              ruleGroup(
                        ruleOnlyTerminal('a'), ruleOnlyTerminal('b'), ruleOnlyTerminal('c'),
                        ruleOnlyTerminal('d'), ruleOnlyTerminal('e'), ruleOnlyTerminal('f'),
                        ruleOnlyTerminal('g'), ruleOnlyTerminal('h'), ruleOnlyTerminal('i'),
                        ruleOnlyTerminal('j'), ruleOnlyTerminal('k'), ruleOnlyTerminal('l'),
                        ruleOnlyTerminal('m'), ruleOnlyTerminal('n'), ruleOnlyTerminal('o'),
                        ruleOnlyTerminal('p'), ruleOnlyTerminal('q'), ruleOnlyTerminal('r'),
                        ruleOnlyTerminal('s'), ruleOnlyTerminal('t'), ruleOnlyTerminal('u'),
                        ruleOnlyTerminal('v'), ruleOnlyTerminal('w'), ruleOnlyTerminal('x'),
                        ruleOnlyTerminal('y'), ruleOnlyTerminal('z'),

                        ruleOnlyTerminal('A'), ruleOnlyTerminal('B'), ruleOnlyTerminal('C'),
                        ruleOnlyTerminal('D'), ruleOnlyTerminal('E'), ruleOnlyTerminal('F'),
                        ruleOnlyTerminal('G'), ruleOnlyTerminal('H'), ruleOnlyTerminal('I'),
                        ruleOnlyTerminal('J'), ruleOnlyTerminal('K'), ruleOnlyTerminal('L'),
                        ruleOnlyTerminal('M'), ruleOnlyTerminal('N'), ruleOnlyTerminal('O'),
                        ruleOnlyTerminal('P'), ruleOnlyTerminal('Q'), ruleOnlyTerminal('R'),
                        ruleOnlyTerminal('S'), ruleOnlyTerminal('T'), ruleOnlyTerminal('U'),
                        ruleOnlyTerminal('V'), ruleOnlyTerminal('W'), ruleOnlyTerminal('X'),
                        ruleOnlyTerminal('Y'), ruleOnlyTerminal('Z'),

                        ruleOnlyTerminal('1'), ruleOnlyTerminal('2'), ruleOnlyTerminal('3'),
                        ruleOnlyTerminal('4'), ruleOnlyTerminal('5'), ruleOnlyTerminal('6'),
                        ruleOnlyTerminal('7'), ruleOnlyTerminal('8'), ruleOnlyTerminal('9'),

                        ruleOnlyTerminal('_'),

                        ruleEnd
                      ),
                      ruleEnd
            ),
            1, 0
       ),
       ruleEnd
    )
),
//Label = Name, ':'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Label"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleNonterminalByName(&names, "Name"), ruleTerminal(':'), ruleEnd
     ),
     ruleEnd
    )
),
//Labeled list = (Label * -1, Name, ",")*, (Label * -1, Name, "," * -1) * -1
rule(
    (
     ruleOnlyNonterminalByName(&names, "Labeled list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleAny(
             ruleOptional(ruleNonterminalByName(&names, "Label"), ruleEnd),
             ruleNonterminalByName(&names, "Name"),
             ruleTerminal(','),

             ruleEnd
          ),
          ruleOptional(
              ruleOptional(ruleNonterminalByName(&names, "Label"), ruleEnd),
              ruleNonterminalByName(&names, "Name"),
              ruleOptional(ruleTerminal(','), ruleEnd),

              ruleEnd
          ),

          ruleEnd
     ),
     ruleEnd
    )
),
//Labeled value list = '(', Labeled list, ')'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Labeled value list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleTerminal('('),
         ruleNonterminalByName(&names, "Labeled list"),
         ruleTerminal(')'),

         ruleEnd
     ),
     ruleEnd
    )
),
//Labeled jump list = '(', Labeled list, ')'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Labeled jump list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleTerminal('['),
         ruleNonterminalByName(&names, "Labeled list"),
         ruleTerminal(']'),

         ruleEnd
     ),
     ruleEnd
    )
),
//Instruction specification = Name, Labeled value list * -1, Labeled jump list * -1, ';'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Instruction specification"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleNonterminalByName(&names, "Name"),
         ruleOptional(ruleNonterminalByName(&names, "Labeled value list"), ruleEnd),
         ruleOptional(ruleNonterminalByName(&names, "Labeled jump list"), ruleEnd),
         ruleTerminal(';'),

         ruleEnd
     ),
     ruleEnd
    )
),
//Instruction definition body = '{', (Label * -1, (Instruction specification | Instruction Definition))* '}'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Instruction definition body"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleTerminal('{'),
         ruleAny(
             ruleOptional(ruleNonterminalByName(&names, "Label"), ruleEnd),
             ruleGroup(
                 ruleOnlyNonterminalByName(&names, "Instruction specification"),
                 ruleOnlyNonterminalByName(&names, "Instruction definition"),

                 ruleEnd
             ),

             ruleEnd
         ),
         ruleTerminal('}'),

         ruleEnd
     ),
     ruleEnd
    )
),
//Unlabeled list = (name, ',')*, (name, ',' * -1) * -1
rule(
    (
     ruleOnlyNonterminalByName(&names, "Unlabeled list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleAny(
             ruleNonterminalByName(&names, "Name"),
             ruleTerminal(','),

             ruleEnd
         ),
         ruleOptional(
             ruleNonterminalByName(&names, "Name"),
             ruleOptional(ruleTerminal(','), ruleEnd),

             ruleEnd
         ),

         ruleEnd
     ),
     ruleEnd
    )
),
//Unlabeled value list = '(', Unlabeled list, ')'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Unlabeled value list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleTerminal('('),
         ruleNonterminalByName(&names, "Unlabeled list"),
         ruleTerminal(')'),

         ruleEnd
     ),
     ruleEnd
    )
),
//Unlabeled jump list = '(', Unlabeled list, ')'
rule(
    (
     ruleOnlyNonterminalByName(&names, "Unlabeled jump list"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleTerminal('['),
         ruleNonterminalByName(&names, "Unlabeled list"),
         ruleTerminal(']'),

         ruleEnd
     ),
     ruleEnd
    )
),
rule(
    (
     ruleOnlyNonterminalByName(&names, "Instruction definition header"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleNonterminalByName(&names, "Unlabeled value list"),
         ruleNonterminalByName(&names, "Unlabeled jump list"),

         ruleEnd
     ),
     ruleEnd
    )
),
rule(
    (
     ruleOnlyNonterminalByName(&names, "Instruction definition"),
     ruleEnd
    ),
    (
     ruleDefinitionOnce(
         ruleNonterminalByName(&names, "Instruction definition header"),
         ruleNonterminalByName(&names, "Instruction definition body"),

         ruleEnd
     ),
     ruleEnd
    )
),
ruleEnd
);
