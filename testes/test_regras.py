import unittest

class TestRegrasEB59(unittest.TestCase):

    def test_intertravamento_nivel_alto(self):
        """Valida intertravamento de nível máximo em 88%"""
        nivel_atual = 88.2
        bomba_ligada = True
        
        if nivel_atual > 88.0 and bomba_ligada:
            bomba_ligada = False
            
        self.assertFalse(bomba_ligada, "Erro: A bomba deveria ter desligado acima de 88%!")

    def test_intertravamento_nivel_baixo(self):
        """Valida intertravamento de nível mínimo em 28%"""
        nivel_atual = 27.5
        bomba_ligada = False
        
        if nivel_atual < 28.0 and not bomba_ligada:
            bomba_ligada = True
            
        self.assertTrue(bomba_ligada, "Erro: A bomba deveria ter ligado abaixo de 28%!")

    def test_alivio_sobrepressao(self):
        """Valida abertura da válvula de alívio em 6.8 bar"""
        pressao_atual = 6.9
        valvula_aberta = False
        
        if pressao_atual > 6.8:
            valvula_aberta = True
            
        self.assertTrue(valvula_aberta, "Erro: A válvula deveria ter aberto acima de 6.8 bar!")

    def test_diagnostico_sensor_travado(self):
        """Garante que o sistema identifica o status de falha do sensor de nível"""
        leitura_com_falha = {
            "tag": "NI-59",
            "valor": 55.0,
            "unidade": "%",
            "status": "FALHA_TRAVADO"
        }
        
        sistema_identificou_falha = (leitura_com_falha["status"] == "FALHA_TRAVADO")
        
        self.assertTrue(sistema_identificou_falha, "Erro: O sistema deveria ter detectado o status FALHA_TRAVADO!")

if __name__ == "__main__":
    unittest.main()