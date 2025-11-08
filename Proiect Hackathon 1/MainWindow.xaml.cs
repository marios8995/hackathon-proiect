using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Data.SqlClient;


namespace Proiect_Hackathon_1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
       
        public MainWindow()
        {
            InitializeComponent();
            Random rand = new Random();
            double nivelTomberonCurent = rand.Next(10, 98); // Nivel între 10% și 98%

            // 2. Setează nivelul de umplere
            SetNivelUmplereTomberon(nivelTomberonCurent);

        }
        public void SetNivelUmplereTomberon(double procentUmplere)
        {
            // Verifică dacă elementele au fost inițializate
            if (BaraDeUmplere == null || ProcentajTomberon == null)
            {
                // Aceasta este o măsură de siguranță, nu ar trebui să se întâmple
                return;
            }

            // Asigură-te că procentajul este între 0 și 100
            procentUmplere = Math.Max(0, Math.Min(100, procentUmplere));

            // 1. Actualizează TextBlock-ul cu procentajul
            ProcentajTomberon.Text = $"Nivel: {procentUmplere:F0}%";

            // 2. Calculează înălțimea barei de umplere

            // Găsim Border-ul (părintele barei) pentru a obține înălțimea maximă disponibilă
            // Nota: Putem folosi un obiect Wrapper (un StackPanel sau Grid) pentru a afla inaltimea.
            // În acest caz, vom folosi înălțimea Border-ului. Presupunem că Grid.Row="1" are ~300px înălțime.
            double inaltimeMaximaBara = 300;

            // Caută elementul Grid care este părintele lui Rectangle (dacă este necesar)
            if (BaraDeUmplere.Parent is Grid parentGrid)
            {
                // Înălțimea maximă a barei este înălțimea Border-ului.
                // Folosim ActualHeight pentru a obține dimensiunea după ce layout-ul s-a așezat.
                if (parentGrid.Parent is Border borderContainer)
                {
                    // Folosim valoarea vizuală actuală a containerului
                    inaltimeMaximaBara = borderContainer.ActualHeight;
                }
            }
            // Dacă nu putem obține ActualHeight (în constructor), folosim o valoare fixă.
            if (inaltimeMaximaBara == 0)
            {
                // Valoare estimată care se potrivește cu Height="400" al Grid-ului
                inaltimeMaximaBara = 300;
            }

            // Setează înălțimea barei
            BaraDeUmplere.Height = (procentUmplere / 100.0) * inaltimeMaximaBara;

            // 3. Logica de schimbare a culorii (Verde Ecosistem)
            if (procentUmplere > 85)
            {
                // Peste 85% -> Roșu (Alertă)
                BaraDeUmplere.Fill = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#F44336"));
            }
            else if (procentUmplere > 50)
            {
                // Între 50% și 85% -> Galben/Portocaliu (Mediu)
                BaraDeUmplere.Fill = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#FFC107"));
            }
            else
            {
                // Sub 50% -> Verde (Bine/Ecologic)
                BaraDeUmplere.Fill = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#4CAF50"));
            }
        }
    }
}