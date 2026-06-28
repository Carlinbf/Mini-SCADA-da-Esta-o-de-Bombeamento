import unittest

class TestRegrasEB59(unittest.TestCase):

    def test_intertravamento_nivel_alto(self):
        """Regra 1: O nível ultrapassou 88% -> Bomba deve ser desligada."""
        nivel_atual = 88.5
        bomba_ligada = True
        
        if nivel_atual > 88.0 and bomba_ligada:
            bomba_ligada = False
            
        self.assertFalse(bomba_ligada, "Erro: A bomba deveria ter sido desligada em nível alto!")

    def test_intertravamento_nivel_baixo(self):
        """Regra 2: O nível caiu abaixo de 28% -> Bomba deve ligar para reabastecer."""
        nivel_atual = 25.0
        bomba_ligada = False
        
        if nivel_atual < 28.0 and not bomba_ligada:
            bomba_ligada = True
            
        self.assertTrue(bomba_ligada, "Erro: A bomba deveria ter sido ligada em nível baixo!")

    def test_alivio_pressao_maxima(self):
        """Regra 3: Pressão acima de 6.8 bar -> Válvula de alívio deve abrir."""
        pressao_atual = 7.1
        valvula_aberta = False
        
        if pressao_atual > 6.8:
            valvula_aberta = True
            
        self.assertTrue(valvula_aberta, "Erro: A válvula de alívio deveria ter aberto com sobrepressão!")

if _name_ == "_main_":
    unittest.main()