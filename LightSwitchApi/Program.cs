var builder = WebApplication.CreateBuilder(args);

var app = builder.Build();

app.UseHttpsRedirection();

app.MapGet("/lightSwitchState", () =>
{
    return LoadStatus();
})
.WithName("GetLightSwitchState");

app.MapPost("/lightSwitchState/toggle", () =>
{
    var currStatus = LoadStatus();
    var newStatus = !currStatus;
    SaveStatus(newStatus);

    return newStatus;
})
.WithName("ToggleLightSwitchState");

app.Run();

static void SaveStatus(bool flag)
{
    File.WriteAllText("flag.txt", flag.ToString());
}

static bool LoadStatus()
{
    var loadedFlag = true;

    if (File.Exists("flag.txt"))
    {
        string content = File.ReadAllText("flag.txt");
        if (bool.TryParse(content, out bool result))
        {
            loadedFlag = result;
        }
    }

    return loadedFlag;
}